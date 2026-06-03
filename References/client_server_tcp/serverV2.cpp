

#include <bits/stdc++.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <iostream>
    #include <cstring>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

using namespace std;

int main(void)
{
    // 1 - Creazione socket con funzione socket(dominio, tipo, protocollo)
    int fd = socket(AF_INET, SOCK_STREAM, 0);  // AF_INET per ipv4, SOCK_STREAM per TCP, 0 per lasciare scegliere al sistema 
                                                  // il protocollo migliore
    
    // Controllo errore creazione socket

    if(fd == -1){
        perror("socket:");
        return -1;
    }

    // 2 - Assegnazione dell 'indirizzo

    sockaddr_in server{};

    server.sin_family = AF_INET;   // Specifca per ipv4
    server.sin_port = htons(54000);  // Conversione in Big-Endian ( Network Byte Order ) e assegnazione della porta
    server.sin_addr.s_addr = INADDR_ANY;   // Accettazione da qualunque tipo di scheda di rete

    int status = bind(fd, (struct sockaddr *)&server, sizeof(server));

    /*
    bind() è progettata per essere generica e accetta un puntatore a una struttura chiamata sockaddr. 
    Tuttavia, poiché noi stiamo usando IPv4, abbiamo compilato una struttura più specifica chiamata sockaddr_in. 
    Il codice quindi effettua un cast (una conversione forzata) per dire al computer: 
    "Prendi l'indirizzo della mia struttura specifica e trattalo come se fosse quella generica richiesta dalla funzione"
    */
    
    // Controllo errori assegnazione IP
    if(status < 0)
    {
        perror("Bind:");
        return 1;
    }

    // 3 - Ascolto connessioni con listen().

    // listen() restituisce un intero quindi deve essere associata ad una variabile, anche poi per il controllo
    // listen() accetta due parametri:
    //      1 - file descriptor del socket
    //      2 - il backlog, ovvero il numero massimo di connessioni che tiene in attesa prima di rifiutare le nuove

    int listenStatus = listen(fd, 10);

    std::cout << "Server in ascolto sulla porta 54000..." << std::endl;

    if (listenStatus < 0)
    {
        perror("listen:");
        return 1;
    }
    
    // 4 - Accettare la connessione - in loop per permettere di gestire più connessioni

    // Creazione struct di chi si connette, da lasciare vuota

    while (true) {
    sockaddr_in their_addr{};

     /*
    sizeof restituisce un numero (unsigned long), 
    mentre la funzione si aspetta l'indirizzo di memoria (socklen_t *) di una variabile che contenga quella dimensione.
    */
   
    socklen_t addr_size = sizeof(their_addr);

    int clientFd = accept(fd, (struct sockaddr *)&their_addr, &addr_size);
    if (clientFd < 0) {
        perror("accept");
        continue;  // errore su questo client, ma il server continua
    }

    std::cout << "Client connesso!" << std::endl;

    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));

    int bytesRicevuti = recv(clientFd, buffer, sizeof(buffer), 0);
    if (bytesRicevuti > 0) {
        std::cout << "Messaggio ricevuto:\n" << buffer << std::endl;
        char res[] = "200 OK - end communication";
        send(clientFd, res, sizeof(res), 0);
    }

    close(clientFd);  // chiudi il client, poi torna ad accept()
}
    close(fd);
    return 0;
    
}