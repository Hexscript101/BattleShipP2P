/*
    Authors: Luchetta Fabio
    Start date : 3/06/2026
    Explanation: A peer-to-peer networked Battleship game written in C, 
    built on top of an existing local two-player implementation. 
    Two machines connect over TCP and play against each other — each client manages its own game state locally, 
    exchanging only attack coordinates and hit results over the network.
    Useful links: https://github.com/Hexscript101/BattleShipP2P
*/

#include "functions.h"

// Global var
char esemple[LIM][LIM] =  {0};
char boardG1M[LIM][LIM] = {0}; // G1M = griglia giocatore 1 principale, dove verranno segnate le navi. S = griglia di appoggio mosse
char boardG1S[LIM][LIM] = {0};
char boardG2M[LIM][LIM] = {0};
char boardG2S[LIM][LIM] = {0};
char direzioni[4] = {'A', 'V', '<', '>'};

int celleRimanentiG1 = TOTAL_SHIP_CELLS;
int celleRimanentiG2 = TOTAL_SHIP_CELLS;
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

void print_board_numbered(char matr[LIM][LIM])
{
    // Stampa intestazione colonne
    printf("   ");
    for (int c = 0; c < LIM; c++)
    {
        printf("%1d ", c + 1);
    }
    printf("\n");

    // Stampa board con numerazione righe
    for (size_t r = 0; r < LIM; r++)
    {
        printf("%2d ", (int)(r + 1));
        for (size_t c = 0; c < LIM; c++)
        {
            if (matr[r][c] == '+' || matr[r][c] == 'A' || matr[r][c] == 'V' || matr[r][c] == '<' || matr[r][c] == '>'|| matr[r][c] == '?')
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
            for (int i = IpoR; i < (IpoR+dimShip); i++)
            {
                if(matr[i][IpoC] != '~'){
                    flag = 1;
                }
            } 
        }while(matr[IpoR][IpoC] != '~' || flag == 1);

        matr[IpoR][IpoC] = 'A';
        // Ciclo per assegnare dinamicamente le posizioni
        for (int i = IpoR+1; i < (IpoR+dimShip); i++)
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
                
                for (int i = IpoR; i > (IpoR-dimShip); i--)
                {
                    if(matr[i][IpoC] != '~'){
                        flag = 1;
                    }
                }
            } while (matr[IpoR][IpoC] != '~' || flag == 1);

            matr[IpoR][IpoC] = 'V';

            for (int i = IpoR-1; i >= (IpoR-dimShip+1); i--)
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

                for (int i = IpoC; i < (IpoC+dimShip); i++)
                {
                    if(matr[IpoR][i] != '~'){
                        flag = 1;
                    }
                }
            } while (matr[IpoR][IpoC] != '~' || flag == 1);

            matr[IpoR][IpoC] = '<';

            for (int i = IpoC+1; i < (IpoC+dimShip); i++)
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

                for (int i = IpoC; i > (IpoC-dimShip); i--)
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
    printf("                Battle ship v2.1.0                     \n");
    printf("==================================================\n\n");
    printf("Each player gets a randomly placed fleet on a 10×10 grid with: \n\t-2 Frigates\n\t-1 Submarine\n\t-1 Battleship\n\t-1 Aircarrier \n\n");
}

int check_cell(char matr[LIM][LIM], int riga, int colonna)
{
    return (matr[riga][colonna] == '~') ? 0 : 1;
}

void win(int playerVincitore)
{
    clean_up();
    printf("\n===========Player %d has won!=============\n", playerVincitore);
}
// Validazione IPv4
bool validate_ipv4(const char* ip) {
    int a, b, c, d;
    if (sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) != 4) return false;
    return (a >= 0 && a <= 255 && b >= 0 && b <= 255 && 
            c >= 0 && c <= 255 && d >= 0 && d <= 255);
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
        return FAIL;
    }
    return fd;
}

int set_up_client(int fd, int port, char address[])
{
    int res;

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
    printf("\nWaiting on port %d for the other player to connect...\n", PORT);
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
    fdCon = accept(fd, NULL, NULL);
    return fdCon;
}


//Takes two int, convert them in char and send them to the other player 
int send_attack(int row, int colum, int fdCon)  // --> returs error or OK code 
{
    int flag;
    char crow = (char)row;
    char ccolum = (char)colum;

    printf("\nSending attack...\n");

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
int recv_char(int fdCon) // --> return code error if error and the data receved as a int if OK
{
    char chFromNet;
    int flag;

    printf("\nReceving response...\n");

    int recved = recv(fdCon, &chFromNet, sizeof(chFromNet), 0);
    fprintf(stderr, "recved: %d, char: %d\n", recved, (int)chFromNet); //? Debug
    int dataFromNet = (int)chFromNet;

    if (recved > 0)
    {
        flag = dataFromNet;
    }else{
        flag = recved;
    }
    return flag;
}

int send_status(int PersonalFD, int ris)
{
    char risChar = (char)ris;

    int check = send(PersonalFD, &risChar, sizeof(risChar),0);

    if (check == FAIL)
    {
        perror("Problems with the \"send_status\" func: " );
    }
    return check;
}

int game_loop(int player, int PersonalFD) // --> return code : 0 if in progress, 1 if player 1 wins, 2 if player 2 wins.
{
    int turno = 1;
    while (celleRimanentiG1 != 0 && celleRimanentiG2 != 0)
    {

        sleep(TEMPO);
        clean_up();
        if (player == 1 && (turno % 2 == 1)) // Server attacks
        {

            printf("turno: %d\n", turno);

            int row, colum;
            printf("\n-----------------YOUR SHIPS-----------------\n\n");
            print_board_numbered(boardG1M);
            printf("\n--------------YOUR PREVIOUS MOVE--------------\n\n");
            print_board_numbered(boardG1S);
            do // taking input
            {
                printf("\nInsert the row (1 - 10 ): \n>");
                if (scanf("%d", &row) != 1) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                    row = -1;
                    continue;
                }
                row--;
                printf("\nInsert the colum ( 1 - 10 ): \n>");
                if (scanf("%d", &colum) != 1) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                    colum = -1;
                    continue;
                }
                colum--;
            } while ((row < 0 || row >= 10) || (colum < 0 || colum >= 10));
            int check = send_attack(row, colum, PersonalFD);
            if (check == FAIL)
            {   
                perror("\nProblems with the \"send_attack\" func: " );
            }
            int ris = (recv_char(PersonalFD));

            if (ris == 0)
            {
                printf("\nWater!\n");
                boardG1S[row][colum] = 'O';
            }else{
                printf("\nHit!\n");
                boardG1S[row][colum] = 'X';
                scoreG1++;
                celleRimanentiG2--;
            }
            turno++;
        }
        else if(player == 1 && (turno % 2 == 0)) // Server "defends" (receives P2's attack)
        {
            printf("turno: %d\n", turno);

            int row = recv_char(PersonalFD);
            int colum = recv_char(PersonalFD);

            int ris = check_cell(boardG1M, row, colum);
            if (ris == 1) // solo se mi hanno colpito faccio qualcosa, ovvero aggirono la mia main board
            {
                boardG1M[row][colum] = 'X';
            }
            int status_send = send_status(PersonalFD, ris);
            turno++;
            if (status_send <= 0)
            {
                perror("Network error in defend phase");
                return FAIL;
            }


        }
        else if(player == 2 && (turno % 2 == 0)) // client attacks
        {
            printf("turno: %d\n", turno);

            int row,colum;
            printf("\n-----------------YOUR SHIPS-----------------\n\n");
            print_board_numbered(boardG2M);
            printf("\n--------------YOUR PREVIOUS MOVE--------------\n\n");
            print_board_numbered(boardG2S);
            do // taking input
            {
                printf("\nInsert the row (1 - 10 ): \n>");
                if (scanf("%d", &row) != 1) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                    row = -1;
                    continue;
                }
                row--;
                printf("\nInsert the colum ( 1 - 10 ): \n>");
                if (scanf("%d", &colum) != 1) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                    colum = -1;
                    continue;
                }
                colum--;
            } while ((row < 0 || row >= 10) || (colum < 0 || colum >= 10));
            int check = send_attack(row, colum, PersonalFD);
            if (check == FAIL)
            {   
                perror("\nProblems with the \"send_attack\" func: " );
            }
            int ris = (recv_char(PersonalFD));
            if (ris == 0)
            {
                printf("\nWater!\n");
                boardG2S[row][colum] = 'O';
            }else{
                printf("\nHit!\n");
                boardG2S[row][colum] = 'X';
                scoreG2++;
                celleRimanentiG1--;
            }
            turno++;
        }
        else if (player == 2 && (turno % 2 == 1))
        
        {  // client "defends"
            printf("turno: %d\n", turno);

            fflush(stdout);
            printf("\nWaiting coordinates...\n");
            int row = recv_char(PersonalFD);
            int colum = recv_char(PersonalFD);
            
            int ris = check_cell(boardG2M, row, colum);
            if (ris == 1) // solo se mi hanno colpito faccio qualcosa, ovvero aggirono la mia main board
            {
                boardG2M[row][colum] = 'X';
            }
            int status_send = send_status(PersonalFD, ris);
            turno++;
            if (status_send <= 0)
            {
                perror("Network error in defend phase");
                return FAIL;
            }
        }
        
    }
    // check winner
    int winner;
    if (celleRimanentiG1 == 0)
    {
       winner = 2;
    }else{
        winner = 1;
    }
    return winner;
}