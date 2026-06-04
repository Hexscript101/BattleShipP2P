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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("insufficient arguments , closing...\n");
        return FAIL;
    }else if (strcmp(argv[1], "server") == 0)
    {
        /* If server */
    }else if (strcmp(argv[1], "client") == 0)
    {
        /* if client */
    }else{
        printf("Bad usage: ./main <mod> ( See the README.md for more info )");
        return FAIL;
    }
    win(1);
    
    // close all fd pls
    return OK;
}


