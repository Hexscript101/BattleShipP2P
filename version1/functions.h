#include <stdio.h>
#define LIM 10
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
int send_status(int PersonalFD, int ris);
int game_loop(int player, int PersonalFD);
