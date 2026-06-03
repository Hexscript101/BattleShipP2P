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



int main()
{
    // creo il socket e ottengo un file descriptor
    int sock = socket(AF_INET, SOCK_STREAM, 0);         // AF_INET per ipv4 SOCK_STREM per definire protocollo tcp 
    
    if(sock == -1){
        perror("socket:");
        return -1;
    }

    // Descrivo il server a cui voglio connettermi

    sockaddr_in server{};    // Struct per gestire indirizzi ipv4

    memset(&server, 0, sizeof(server));   // Pulisco prima per sicurezza, pulisco l'indirizzo con &

    server.sin_family = AF_INET;     // sin_familiy è la famiglia di indirzzi, sempre AF_INET perchè usiamo ipv4

    server.sin_port = htons(54000);   // htons() converte la porta nel formato di rete necessario

    inet_pton(AF_INET, "192.168.1.109",&server.sin_addr);  // Funzione che converta la str in byte e la manda all'indirizzo di sin_addr

    // Connessione

    int res = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if(res == -1){
        perror("connect");
        return 1;
    }

    printf("Connessione stabilita\n");

    // Mandare comunuicazione

    char req[] = "GET / HTTP/1.0\r\nHost: exemple.com\r\nConnection: close\r\n\r\n";
    send(sock, req, strlen(req), 0);

    // Ricezione risposta

    char buff[4096];
    int numByte;
    
    while ((numByte = recv(sock, buff, sizeof(buff) - 1, 0)) > 0) {
    buff[numByte] = '\0';
    printf("%s", buff);
}

    close(sock);
    return 0;

}
    
