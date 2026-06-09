/*
    Authors: Luchetta Fabio, Leonardo Golisano, Edoardo Trifone
    Start date : 3/06/2026
    Explanation: A peer-to-peer networked Battleship game written in C, 
    built on top of an existing local two-player implementation. 
    Two machines connect over TCP and play against each other — each client manages its own game state locally, 
    exchanging only attack coordinates and hit results over the network.
    Useful links: https://github.com/Hexscript101/BattleShipP2P
*/

// Libraries
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

#include "functions.h"

// Constants
#define LIM 10          // Limit of the game board
#define SHIPS_NUM 5     // Number of ships
#define TOTAL_CELLS 16  // Sum of the cells of all ships
#define TYPE2 2         // Number of the ships made by 2 cell 
#define TEMPO 3         //? Da tenere ? 

#define OK 0
#define FAIL -1

// Colors
#define RESET "\033[0m"
#define GRAY "\033[90m"

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


int main(int argc, char *argv[])
{
    srand(time(NULL));
    char address[16];
    int port, exitCode;
    int turno = 1;
    int winner;
    if (argc != 2)
    {
        printf("insufficient arguments , closing...\n");
        return FAIL;
    }else if (strcmp(argv[1], "server") == 0)
    {
        /* If server */
        // Il server sarà sempre il player 1
        int fdCon = set_up_server();
        if (fdCon == FAIL)
        {
            fprintf(stderr, "SOCK ERROR");
            return FAIL;
        }
        title();
        // GAME
        // non visible prep
        init_board(boardG1M);
        memset(boardG1S, '?', sizeof(boardG1S));
        gen_ships(boardG1M);
        

        // GAME LOOP --------------------------------- 
        winner = game_loop(1, fdCon);
        win(winner);
        close(fdCon);
        
        
    }else if (strcmp(argv[1], "client") == 0)
    {
        /* if client */
        // il client sarà sempre il player 2
        title();
        printf("ip address:  (ex 192.169.1.1): \n>");
        fgets(address, sizeof(address), stdin);
        address[strcspn(address, "\n")] = '\0';
        printf("Port: \n>");
        scanf("%d", &port);
        getchar();

        // Real start 
        int fd = create_socket();
        int res = set_up_client(fd, port, address);
        if (res == FAIL)
        {
            perror("sock: ");
            return FAIL;
        }

        // GAME
        // non visible prep
        init_board(boardG2M);
        memset(boardG2S, '?', sizeof(boardG2S));
        gen_ships(boardG2M);

        winner = game_loop(2, fd);
        win(winner);
        close(fd);
        
    }else{
        printf("Bad usage: ./main <mod> ( See the README.md for more info )");
        return FAIL;
    }
    return OK;
}


