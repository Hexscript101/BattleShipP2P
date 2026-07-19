/*
    Authors: Luchetta Fabio 
    Start date : 3/06/2026
    Explanation: A peer-to-peer networked Battleship game written in C, 
    built on top of an existing local two-player implementation. 
    Two machines connect over TCP and play against each other — each client manages its own game state locally, 
    exchanging only attack coordinates and hit results over the network.
    Useful links: https://github.com/Hexscript101/BattleShipP2P
*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Constants
#define LIM 10
#define SHIPS_NUM 5
#define TOTAL_SHIP_CELLS 16
#define TYPE2 2
#define TEMPO 6
#define PORT 4444

#define OK 0
#define FAIL -1

// Colors
#define RESET "\033[0m"
#define GRAY  "\033[90m"

// Global variables (defined in func.c)
extern char esemple[LIM][LIM];
extern char boardG1M[LIM][LIM];
extern char boardG1S[LIM][LIM];
extern char boardG2M[LIM][LIM];
extern char boardG2S[LIM][LIM];
extern char direzioni[4];

extern int celleRimanentiG1;
extern int celleRimanentiG2;
extern int scoreG1;
extern int scoreG2;

// Base funcs
void clean_up();
void init_board(char matr[LIM][LIM]);
void print_board(char matr[LIM][LIM]);
void print_board_numbered(char matr[LIM][LIM]);
void place_type2(char matr[LIM][LIM]);
void place_other(char matr[LIM][LIM], int dimShip);
void gen_ships(char matr[LIM][LIM]);
void title();
int check_cell(char matr[LIM][LIM], int riga, int colonna);
void win(int playerVincitore);

// Network funcs
int create_socket();
int set_up_client(int fd, int port, char address[]);
int set_up_server();
int send_attack(int row, int colum, int fdCon);
int recv_char(int fdCon);
int send_status(int PersonalFD, int ris);
int game_loop(int player, int PersonalFD);

#endif