/*
-   funzioni di rete:
        -   connect
        -   listen
        -   protocollo
*/

#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>

#include "functions.h"

#define LIM 10
#define SHIPS_NUM 5
#define TOTAL_CELLS 16
#define TYPE2 2
#define TEMPO 3
#define PORT 4444

#define OK 0
#define FAIL -1

// Colors
#define RESET "\033[0m"
#define GRAY "\033[90m"

// Base funcs
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
// New network funcs
int create_socket();
int set_up_client(int fd,int port, char address[]);
int set_up_server();
int send_attack(int row, int colum, int fdCon);
int recv_char(int fdCon);

// Global var
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
        system("clear");
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
    printf("                Battaglia navale v2.1.0                     \n");
    printf("==================================================\n\n");
    printf("Nota: Giocherai in una griglia 10x10 con a disposizone: \n\t-2 Fregate\n\t-1 Sottomarino\n\t-1 Corazzata\n\t-1 Portaaerei \n\n");
}

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

// --------------------------------------------------------------------------

// Create a TCP socket for ipv4 connection. To use once for all the game.
int create_socket()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        perror("sock: ");
    }
    return fd;
}

int set_up_client(int fd, int port, char address[])
{
    int flag, res;

    // Define the other player info for the connetc() func
    struct sockaddr_in otherPlayer;

    otherPlayer.sin_family = AF_INET;
    otherPlayer.sin_port = htons(port);
    inet_pton(AF_INET, address, &otherPlayer.sin_addr );

    res = connect(fd, (struct sockaddr *)&otherPlayer, sizeof(otherPlayer));

    return res;
}


// Per il server: create_socket()restituisce l'fd del socket in ascolto, quello da passare a bind()/listen(). 
// Dopo accept() ne avremo uno nuovo, che è quello da usare per tutta la partita e che si passa a protocol()

int set_up_server()
{
    int fd,status, listenStatus, fdCon;
    fd = create_socket();

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    status = bind(fd, (struct sockaddr *)&server, sizeof(server));

    if(status < 0 ){
        perror("Bind: ");
        return FAIL;
    }

    listenStatus = listen(fd, 1);

    if(listenStatus < 0){
        perror("Listen: ");
        return FAIL;
    }

    printf("Waiting on port %d for the other player to connect...", PORT);

    fdCon = accept(fd, NULL, NULL);

    return fdCon;

}

/*
    send_attack(row, col, fd) — manda le coordinate
    recv_result(fd) — riceve il risultato
    recv_attack(fd) — riceve le coordinate dell'avversario
    send_result(row, col, fd) — manda il risultato
*/

int send_attack(int row, int colum, int fdCon)
{
    int flag;
    char crow = (char)row;
    char ccolum = (char)colum;

    int s1 = send(fdCon, &crow, sizeof(crow), 0);
    int s2 = send(fdCon, &ccolum, sizeof(ccolum), 0);

    if (s1 > 0 && s2 > 0)
    {
        flag = OK;
    }else{
        flag = FAIL;
    }
    return flag;
}

// Take a generic char ( rows or colums ), store it in a var, converts it and returns it
int recv_char(int fdCon)
{
    char chFromNet;
    int flag;

    int recved = recv(fdCon, &chFromNet, sizeof(chFromNet), 0);

    int dataFromNet = (int)chFromNet;

    if (recved > 0)
    {
        flag = dataFromNet;
    }else{
        flag = recved;
    }
    return flag;
}