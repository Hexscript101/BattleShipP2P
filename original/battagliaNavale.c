/*
    Autore: Luchetta Fabio
    Classe: 3 C INF IIS A.Avogadro
    Data di inizio progettazione: 04/04/2026 
    Consegna: 
        Gioca a battaglia navale con una compagna/un compagno di classe

        Aiutandoti con schemi e il gioco che hai appena vinto,
        modella la griglia di gioco e scrivi una funzione che data una matrice, l'inizializza randomicamente con il campo di battaglia. Generando le barche in pozioni e orientamenti casuali.

        La matrice 10x10 dovrà contenere:
        - 2 barche da 2
        - 1 barca da 3
        - 1 barca da 4
        - 1 barca da 5

        Il nostro programma dovrà permetterci di segnare i punti che abbiamo colpito, 
        le barche che abbiamo colpito e se la barca è affondata o no.

    Riferimenti utili:
    https://it.wikipedia.org/wiki/Battaglia_navale_(gioco)


*/

// Librerie
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
// Costanti
#define LIM 10
#define SHIPS_NUM 5
#define TOTAL_CELLS 16
#define TYPE2 2
#define TEMPO 3
// Colori
#define RESET "\033[0m"
#define GRAY "\033[90m"

// Prototipazione
void clean_up();
void init_board(char matr[LIM][LIM]);
void print_board(char matr[LIM][LIM]);
void place_type2(char matr[LIM][LIM]);
void place_other(char matr[LIM][LIM], int dimShip);
void gen_ships(char matr[LIM][LIM]);
void title();
int check_cell(char matr[LIM][LIM], int riga, int colonna);
void input(int playerAttaccante);
void win(int playerVincitore);

// Variabili globali 
char esemple[LIM][LIM] =  {0};
char boardG1M[LIM][LIM] = {0}; // G1M = griglia giocatore 1 principale, dove verranno segnate le navi. S = griglia di appoggio mosse
char boardG1S[LIM][LIM] = {0};
char boardG2M[LIM][LIM] = {0};
char boardG2S[LIM][LIM] = {0};
char direzioni[4] = {'A', 'V', '<', '>'}; 

int celleRimanentiG1 = TOTAL_CELLS;
int celleRimanentiG2 = TOTAL_CELLS;
int scoreG1 = 0;
int scoreG2 = 0;

void clean_up()
{
    #ifdef _WIN32
        system("cls");
    #elif __linux__
        system("clear");
    #endif
}

void init_board(char matr[LIM][LIM])
{
    for (size_t r = 0; r < LIM; r++)
    {
        for (size_t c = 0; c < LIM; c++)
        {
            matr[r][c] = '~';
        }
    }
}

void print_board(char matr[LIM][LIM])
{
    for (size_t r = 0; r < LIM; r++)
    {
        for (size_t c = 0; c < LIM; c++)
        {
            if (matr[r][c] == '+' || matr[r][c] == 'A' || matr[r][c] == 'V' || matr[r][c] == '<' || matr[r][c] == '>'|| matr[r][c] == '?' )
            {
                printf(GRAY"|%c"RESET, matr[r][c]);
            }
            else
            {
                printf("|%c", matr[r][c]);
            }
        }
        printf("\n");
    }
    
}

//    Funzione che generare due navi da 2 controllando già lo status delle celle necessarie 

void place_type2(char matr[LIM][LIM])
{
    for (size_t i = 0; i < TYPE2; i++)
    {
        int IpoR;
        int IpoC;
        
        char prua = direzioni[rand() % 4];
        switch (prua)
        {
        case 'A':
            do
            {
                IpoR = rand() % 8;  // Posizione ipotetica della riga
                IpoC = rand() % 9;  // posizone ipotetica della colonna
            }while(matr[IpoR][IpoC] != '~' || matr[IpoR+1][IpoC] != '~');

            matr[IpoR][IpoC] = 'A';
            matr[IpoR+1][IpoC] = '+';
            break;
        case 'V':
            do
            {
                IpoR = rand() % 8 +1 ;  // Posizione ipotetica della riga
                IpoC = rand() % 9;  // posizone ipotetica della colonna
            }while(matr[IpoR][IpoC] != '~' || matr[IpoR-1][IpoC] != '~');

            matr[IpoR][IpoC] = 'V';
            matr[IpoR-1][IpoC] = '+';
            break;
        case '<':
            do
            {
                IpoR = rand() % 9 ;  // Posizione ipotetica della riga
                IpoC = rand() % (LIM-2);  // posizone ipotetica della colonna
            }while(matr[IpoR][IpoC] != '~' || matr[IpoR][IpoC+1] != '~');

            matr[IpoR][IpoC] = '<';
            matr[IpoR][IpoC+1] = '+';
            break;
        case '>':
            do
            {
                IpoR = rand() % 9 ;  // Posizione ipotetica della riga
                IpoC = rand() % 9 + 1;  // posizone ipotetica della colonna
            }while(matr[IpoR][IpoC] != '~' || matr[IpoR][IpoC-1] != '~');

            matr[IpoR][IpoC] = '>';
            matr[IpoR][IpoC-1] = '+';
            break;
        default:
            break;
        }
    }
}

void place_other(char matr[LIM][LIM], int dimShip)
{
    int IpoR;
    int IpoC;
    int flag = 0;

    char prua = direzioni[rand() % 4];

    switch (prua)
    {
    case 'A':
        do
        {
            flag = 0;
            IpoR = rand() % (LIM-dimShip);
            IpoC = rand() % 9;


            // Controllo in verticale tutte le celle per tutta la lunghezza della nave
            for (size_t i = IpoR; i <= (IpoR+dimShip); i++)
            {
                if(matr[i][IpoC] != '~'){
                    flag = 1;
                }
            } 
        }while(matr[IpoR][IpoC] != '~' || flag == 1);

        matr[IpoR][IpoC] = 'A';
        // Ciclo per assegnare dinamicamente le posizioni
        for (size_t i = IpoR+1; i < (IpoR+dimShip); i++)
            {
                matr[i][IpoC] = '+';
            }
        break;
    case 'V':
            int min = dimShip-1;
            do
            {
                flag = 0;
                IpoC = rand() % 9;
                IpoR = rand() % (9 - min + 1) + min;
                
                for (int i = IpoR; i >= (IpoR-dimShip); i--)
                {
                    if(matr[i][IpoC] != '~'){
                        flag = 1;
                    }
                }
            } while (matr[IpoR][IpoC] != '~' || flag == 1);

            matr[IpoR][IpoC] = 'V';

            for (int i = IpoR-1; i > (IpoR-dimShip); i--)
            {
                matr[i][IpoC] ='+';
            }
            break;
    case '<':
            int max = (9 - dimShip) + 1;    // +1 sarebbe la prua che viene già contata
            do
            {
                flag = 0;
                IpoR = rand() % 9;
                IpoC = rand() % max;

                for (size_t i = IpoC; i <= (IpoC+dimShip); i++)
                {
                    if(matr[IpoR][i] != '~'){
                        flag = 1;
                    }
                }
            } while (matr[IpoR][IpoC] != '~' || flag == 1);

            matr[IpoR][IpoC] = '<';

            for (size_t i = IpoC+1; i < (IpoC+dimShip); i++)
            {
                matr[IpoR][i] = '+';
            }
            break;
    case '>':
            int min2 = dimShip-1; 
            do
            {
                flag = 0;
                IpoR = rand() % 9;
                IpoC = rand() % (9 - min2 + 1) + min2;

                for (int i = IpoC; i >= (IpoC-dimShip); i--)
                {
                    if(matr[IpoR][i] != '~'){
                        flag = 1;
                    }
                }

            } while (matr[IpoR][IpoC] != '~' || flag == 1);

            matr[IpoR][IpoC] = '>';

            for (int i = IpoC-1; i > (IpoC-dimShip); i--)
            {
                matr[IpoR][i] = '+';
            }

            break;
    default:
        break;
    }
}

//    Call all the single funcs to generate all the type of warships

void gen_ships(char matr[LIM][LIM])
{
    place_type2(matr);
    place_other(matr, 3);
    place_other(matr, 4);
    place_other(matr, 5);
}

void title()
{
    printf("==================================================\n\n");
    printf("                Battaglia navale v1.0.0                     \n");
    printf("==================================================\n\n");
    printf("Nota: Giocherai in una griglia 10x10 con a disposizone: \n\t-2 Fregate\n\t-1 Sottomarino\n\t-1 Corazzata\n\t-1 Portaaerei \n\n");
}

// 0 = mancato 1 = colpito

int check_cell(char matr[LIM][LIM], int riga, int colonna)
{
    return (matr[riga][colonna] == '~') ? 0 : 1;
}

void input(int playerAttaccante)
{
    int riga,colonna,ris;
    do
    {
        printf("Sistemi d'arma pronti a fare fuoco\n");
        printf("Inserire coordinata riga ( Partendo da 1 ):\n");
        scanf("%d", &riga);
        riga--;
        printf("Inserire coordinata colonna ( Partendo da 1 ):\n");
        scanf("%d", &colonna);
        colonna--;
    } while (riga < 0 || riga >10 || colonna < 0 || colonna > 10);
    
    if (playerAttaccante == 1)
    {
        ris = check_cell(boardG2M, riga,colonna);

        if (ris == 0)
        {
            boardG1S[riga][colonna] = 'O';
            printf("Mancato!\n");
        }else
        {
            boardG1S[riga][colonna] = 'X';  // Aggiorno tabellone di supporto di chi attacca
            boardG2M[riga][colonna] = 'X'; // Aggiorno tabellone principale di chi viene attaccato
            scoreG1++;
            celleRimanentiG2--;
            printf("Colpito!\n");
        }
        
    }
    else // Attacca giocatore 2
    {
       ris = check_cell(boardG1M, riga,colonna);

        if (ris == 0)
        {
            boardG2S[riga][colonna] = 'O';
            printf("Mancato!\n");
        }else
        {
            boardG2S[riga][colonna] = 'X';  // Attaccante segna sul supporto
            boardG1M[riga][colonna] = 'X';  // Attaccato viene segnato sul principale
            scoreG2++;
            celleRimanentiG1--;
            printf("Colpito!\n");
        } 
    }
}

void win(int playerVincitore)
{
    clean_up();
    printf("\n===========Il giocatore %d ha vinto!=============\n", playerVincitore);
}

int main(void)
{
    srand(time(NULL));

    // Fase preparatoria
    init_board(boardG1M);
    init_board(boardG2M);
    init_board(esemple);
    memset(boardG1S, '?', sizeof(boardG1S));
    memset(boardG2S, '?', sizeof(boardG2S));
    gen_ships(boardG1M);
    gen_ships(boardG2M);
    title();
    sleep(TEMPO);
    

    int turno = 0;
    int exit = 0;
    int winner = 0;

    
    // Game loop 
    // turno 0 - giocatore 1
    while (exit == 0)
    {
        sleep(TEMPO);
        clean_up();
        if(turno % 2 == 0)
        {
            printf("==============Turno giocatore 1 ===========\n");
            printf("Le tue navi:\n");
            print_board(boardG1M);
            printf("Le tue mosse:\n");
            print_board(boardG1S);
            printf("Punteggio attuale: %d\n", scoreG1);
            input(1);
            turno++;
        }
        else{
            printf("==============Turno giocatore 2 ===========\n");
            printf("Le tue navi:\n");
            print_board(boardG2M);
            printf("Le tue mosse:\n");
            print_board(boardG2S);
            printf("Punteggio attuale: %d\n", scoreG2);
            input(2);
            turno++;
        }
        if (scoreG1 == TOTAL_CELLS || scoreG2 == TOTAL_CELLS)
        {
            winner = (scoreG1 > scoreG2) ? 1 : 2;
            exit = 1;
        }
    }
    win(winner);
    return 0;
}
