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

int main(int argc, char *argv[])
{
    srand(time(NULL));
    char address[46];  // margine per indirizzi IPv4/IPv6, non solo i 15 char minimi di IPv4
    int port;
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
            fprintf(stderr, "SOCK ERROR\n");
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
        char address[IP_MAX_LEN + 1];
        printf("IP address: ");
        if (fgets(address, sizeof(address), stdin) == NULL) {
            fprintf(stderr, "Error reading IP address\n");
	        return FAIL;
        }
        address[strcspn(address, "\n")] = '\0';

        if (!validate_ipv4(address)) {
            fprintf(stderr, "Invalid IPv4 address\n");
            return FAIL;
        }
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
        printf("Bad usage: ./main <mod> ( See the README.md for more info )\n");
        return FAIL;
    }
    return OK;
}
