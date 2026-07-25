# Code Review & Architectural Recommendations
*Senior Developer Perspective - 20+ Years Experience*

---

## Executive Summary

Questo progetto implementa un gioco Battleship P2P in C, ma presenta numerosi problemi architetturali e di design che ne limitano la manutenibilità, la sicurezza e la scalabilità. Le raccomandazioni qui sotto sono organizzate per priorità e includono sia correzioni immediate che riflessioni su come ristrutturare il codice.

---

## 1. ARCHITETTURA: Gestione dello Stato globale

### Problema
```c
// func.c:14-18
char boardG1M[LIM][LIM] = {0};
char boardG1S[LIM][LIM] = {0};
char boardG2M[LIM][LIM] = {0};
char boardG2S[LIM][LIM] = {0};
int celleRimanentiG1 = TOTAL_SHIP_CELLS;
int scoreG1 = 0;
```

**Perché è problematico:**
- Gli stati globali rendono impossibile il testing unitario
- Non è possibile eseguire più partite contemporaneamente
- Crea coupling forte tra moduli
- Difficile tracciare il flusso di dati

### Raccomandazione
```c
// Creare una struttura che incapsuli tutto lo stato del gioco
typedef struct {
    char main_board[10][10];      // Board con navi
    char shot_board[10][10];       // Traccia colpi ricevuti
    int remaining_cells;
    int score;
    bool is_sunk;                  // Nuovo flag per stato nave
} Player;

typedef struct {
    Player player1;
    Player player2;
    int current_turn;
    int socket_fd;
    bool game_active;
} GameState;
```

**Benefici:**
- Passa lo stato come parametro alle funzioni
- Rende visibile il flusso di dati
- Permette il testing con stati diversi
- Fondamentale per future estensioni (multi-giocatore, replay, etc.)

---

## 2. PROTOCOLLO DI RETE: Serializzazione debole

### Problema
```c
// func.c:348-366
int send_attack(int row, int colum, int fdCon) {
    char crow = (char)row;
    char ccolum = (char)colum;
    send(fdCon, &crow, sizeof(crow), 0);  // Invia singolarmente
    send(fdCon, &ccolum, sizeof(ccolum), 0);
}
```

**Perché è problematico:**
- I caratteri vengono inviati separatamente, rischiando di arrivare fuori ordine
- Nessun checksum o validazione
- L'intera riga di debug indica problemi nella ricezione
- Non c'è framing protocol (nessun indicatore di fine pacchetto)

### Raccomandazione
```c
#define PACKET_SIZE 3  // [row][col][checksum]

typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t checksum;  // row XOR col
} __attribute__((packed)) AttackPacket;

typedef struct {
    uint8_t status;    // 0=miss, 1=hit, 2=sunk
    uint8_t checksum;  // status XOR turn_count
} __attribute__((packed)) ResponsePacket;
```

**Implementazione corretta:**
```c
int send_attack_packet(int fd, int row, int col) {
    AttackPacket pkt = {
        .row = (uint8_t)row,
        .col = (uint8_t)col,
        .checksum = (uint8_t)(row ^ col)
    };
    
    ssize_t sent = send(fd, &pkt, sizeof(pkt), MSG_NOSIGNAL);
    return (sent == sizeof(pkt)) ? OK : FAIL;
}
```

---

## 3. LOGICA DI TURNO: Struttura complessa e fragile

### Problema
```c
// game_loop:402-548
if (player == 1 && (turno % 2 == 1))           // Attacco P1
else if(player == 1 && (turno % 2 == 0))      // Difesa P1 (RICEVE)
else if (player == 2 && (turno % 2 == 0))     // Attacco P2
else if (player == 2 && (turno % 2 == 1))     // Difesa P2 (RICEVE)
```

**Perché è problematico:**
- La logica è invertita per la difesa: il giocatore "difende" quando in realtà riceve un attacco
- Confusione concettuale tra "attacco" e "ricevi attacco"
- Il turno incrementa in modo incoerente (una volta in difesa, volte in attacco)
- Difficile capire chi ha l'attivo in ogni momento

### Raccomandazione
Riusare un semplice state machine:

```c
typedef enum {
    STATE_P1_ATTACK,
    STATE_P1_WAIT_RESPONSE,
    STATE_P2_ATTACK,
    STATE_P2_WAIT_RESPONSE
} GamePhase;

GamePhase get_current_phase(int turn) {
    return (turn % 2 == 1) ? STATE_P1_ATTACK : STATE_P2_ATTACK;
}

// Oppure semplicemente: chi è il nostro turno?
bool is_my_turn(int player, int turn) {
    return ((player == 1 && turn % 2 == 1) || 
            (player == 2 && turn % 2 == 0));
}
```

---

## 4. VALIDAZIONE INPUT: Buffer overflow e validazione insufficiente

### Problema
```c
// func.c:53-60
printf("ip address:  (ex 192.169.1.1): \n>");
fgets(address, sizeof(address), stdin);  // sizeof(address) = 16
address[strcspn(address, "\n")] = '\0';
```

**Perché è problematico:**
- `sizeof(address)` in `main.c` è 16, ma indirizzo IPv4 può essere 15 caratteri + null
- L'indirizzo può essere lungo 79 caratteri (es: `192.168.1.100`)
- Nessun controllo del formato IPv4
- `scanf` senza limitatore di lunghezza

### Raccomandazione
```c
#define IP_MAX_LEN 46  // "255.255.255.255" = 15 chars, ma lasciamo margine

// Validazione IPv4
bool validate_ipv4(const char* ip) {
    int a, b, c, d;
    if (sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) != 4) return false;
    return (a >= 0 && a <= 255 && b >= 0 && b <= 255 && 
            c >= 0 && c <= 255 && d >= 0 && d <= 255);
}

// Input sicuro
char address[IP_MAX_LEN + 1];
printf("IP address: ");
if (fgets(address, sizeof(address), stdin) == NULL) {
    // Gestisci errore
}
address[strcspn(address, "\n")] = '\0';

if (!validate_ipv4(address)) {
    fprintf(stderr, "Invalid IPv4 address\n");
    return FAIL;
}
```

---

## 5. ERROR HANDLING: Mancanza di pulizia delle risorse

### Problema
```c
// main.c:57-84
if (argc != 2) {
    printf("insufficient arguments...\n");
    return FAIL;  // OK, ma non chiude nulla
}
```

**Perché è problematico:**
- Nessun pattern RAII o cleanup
- Socket non chiuso in caso di errore
- Nessun logging strutturato
- Errori di rete non gestiti gracelessly

### Raccomandazione
```c
// Usare goto per cleanup centrale (pattern C idiomático)
int main(int argc, char *argv[]) {
    int fd = -1;
    GameState state = {0};
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server|client>\n", argv[0]);
        return FAIL;
    }
    
    if (strcmp(argv[1], "server") == 0) {
        fd = set_up_server();
        if (fd < 0) goto cleanup;
        // ...
    }
    
cleanup:
    if (fd >= 0) close(fd);
    return result;
}
```

---

## 6. MAGIC NUMBERS: Costanti non definite

### Problema
```c
// func.c:103-104
IpoR = rand() % 8;   // Perché 8?
IpoC = rand() % 9;   // Perché 9?

// func.c:425
} while ((row < 0 || row > 10) || (colum < 0 || colum > 10));
```

**Perché è problematico:**
- 8 e 9 sono "magic numbers" per il posizionamento navi
- Il controllo usa `> 10` invece di `>= 10` (off-by-one!)
- Il commento dice "1-10" ma il codice controlla 0-10

### Raccomandazione
```c
#define BOARD_SIZE 10
#define BOARD_MIN 0
#define BOARD_MAX (BOARD_SIZE - 1)

// Per la nave di tipo 2 (2 celle)
#define SHIP_TYPE2_MIN_ROW 0
#define SHIP_TYPE2_MAX_ROW (BOARD_SIZE - 2)  // Lascia spazio per seconda cella

// Validazione
if (row < BOARD_MIN || row > BOARD_MAX || col < BOARD_MIN || col > BOARD_MAX) {
    printf("Invalid coordinates. Must be 1-10.\n");
    continue;
}
```

---

## 7. DUPLICAZIONE DI CODICE: Game loop ripetuto

### Problema
Il codice per l'attacco è duplicato 4 volte (P1 attacco, P2 attacco, e due volte per difesa).

**Perché è problematico:**
- Qualsiasi modifica deve essere fatta in 4 posti
- Bug replicati ovunque
- Manutenzione costosa

### Raccomandazione
```c
typedef struct {
    int row;
    int col;
} Coordinate;

Coordinate get_player_attack(const char* prompt_row, const char* prompt_col) {
    Coordinate coord;
    do {
        printf("%s (1-10): ", prompt_row);
        if (scanf("%d", &coord.row) != 1) {
            // pulisci buffer
        }
        coord.row--;
        printf("%s (1-10): ", prompt_col);
        scanf("%d", &coord.col);
        coord.col--;
    } while (coord.row < 0 || coord.row >= BOARD_SIZE || 
             coord.col < 0 || coord.col >= BOARD_SIZE);
    return coord;
}

int process_attack(GameState* state, int attacker, int defender) {
    Coordinate attack = get_player_attack("Row", "Column");
    
    if (send_attack_packet(state->socket_fd, attack.row, attack.col) != OK) {
        return FAIL;
    }
    
    int result = recv_status(state->socket_fd);
    
    // Aggiorna board...
    return OK;
}
```

---

## 8. CONCORRENZA: Race condition teorica

### Problema
```c
// func.c:377
fprintf(stderr, "recved: %d, char: %d\n", recved, (int)chFromNet);
```

**Perché è problematico:**
- Debug stamp su stderr può interferire con protocollo
- Se il client stampa, potrebbe interferire con la ricezione
- Nessun timeout configurato per `recv()`

### Raccomandazione
```c
// Timeout per recv
struct timeval tv;
tv.tv_sec = 10;  // 10 secondi timeout
tv.tv_usec = 0;

setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
```

---

## 9. DESIGN DELLE NAVI: Rappresentazione confusa

### Problema
```c
// Commenti sparsi
'A' = Frigate (2 cells) - stem cell + '+' connecting cell
'V' = Submarine (3 cells) - stem cell + '+' connecting cells (vertical)
```

**Perché è problematico:**
- La direzione è codificata nel carattere della nave, non nella disposizione
- `<>` indica orizzontale, ma la logica di posizionamento è complessa
- Nessun tracking di quante celle ha una nave

### Raccomandazione
```c
typedef enum { ORIZZONTALLE, VERTICALE } Direzione;

typedef struct {
    char type;              // 'F'=Frigate, 'S'=Submarine, 'B'=Battleship, 'C'=Carrier
    int size;               // 2, 3, 4, 5
    int row;
    int col;
    Direzione dir;
    bool sunk;              // Stato corrente
} Navice;

typedef struct {
    Navice ships[MAX_SHIPS];
    int count;
} Flotta;
```

---

## 10. SICUREZZA: Buffer over-read

### Problema
```c
// func.c:105
}while(matr[IpoR][IpoC] != '~' || matr[IpoR+1][IpoC] != '~');
// Se IpoR = 7, allora IpoR+1 = 8, ma se IpoR = 8, IpoR+1 = 9 (OK)
// Se IpoR = 9, IpoR+1 = 10 -> ACCESSO FUORI BOUNDS!
```

**Perché è problematico:**
- Accesso a `matr[9][c]` e `matr[10][c]` quando IpoR può essere 9
- Il modulo `rand() % 8` limita a 0-7, ma non è chiaro
- La logica `place_other` ha simili problemi

### Raccomandazione
```c
// Aggiungere assertion o validazione
#define SAFE_IDX(i) ((i) >= 0 && (i) < BOARD_SIZE)

// Usare limiti corretti
IpoR = rand() % (BOARD_SIZE - ship_size + 1);  // Per orizzontale
IpoR = rand() % BOARD_SIZE;                   // Per verticale, ma verificare fine
```

---

## 11. MESSAGGI IN ITALIANI: Manca internazionalizzazione

### Problema
Tutti i messaggi sono in italiano:
```c
printf("insufficient arguments , closing...\n");
printf("Insert the row (1 - 10 ): \n>");
```

**Raccomandazione:**
- Usare costanti o file di messaggi
- Preparare per futura internazionalizzazione

---

## 12. TESTABILITÀ: Nessun test

### Problema
Nessun framework di test, nessun modo per verificare automaticamente la correttezza.

### Raccomandazione
```c
#ifdef UNIT_TEST
#include "test_framework.h"

void test_board_init() {
    char board[10][10];
    init_board(board);
    // Verifica che tutte le celle siano '~'
}
#endif
```

---

## Priorità di implementazione

| Priorità | Area | Complessità | Impatto |
|----------|------|-------------|---------|
| CRITICA  | Stato globale → GameState | Media | Alto |
| ALTA     | Protocollo di rete | Media | Alto |
| ALTA     | Validazione input | Bassa | Medio |
| MEDIA    | Ristrutturazione game loop | Alta | Alto |
| MEDIA    | Rimozione magic numbers | Bassa | Medio |
| BASSA    | Aggiunta timeout socket | Bassa | Medio |

---

## Conclusioni

Questo codice è una buona base per un progetto di rete, ma richiede una refactoring significativa per:

1. **Sicurezza**: validazione input, buffer safety
2. **Manutenibilità**: rimozione global state, riduzione duplicazione
3. **Flessibilità**: protocollo ben definito, configurabile
4. **Professionalità**: logging, error handling, testing

Il pattern consigliato è: **incremental refactoring**. Inizia con la creazione della struttura `GameState`, poi sposta verso il protocollo binario, infine pulisci il game loop.