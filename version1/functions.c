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

#define LIM 10
#define SHIPS_NUM 5
#define TOTAL_CELLS 16
#define TYPE2 2
#define TEMPO 3

// Colori
#define RESET "\033[0m"
#define GRAY "\033[90m"

//funzioni di base
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
    printf("                Battaglia navale v1.0.0                     \n");
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
