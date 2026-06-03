#include <bits/stdc++.h>  

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

#define FAIL -1
#define OK 0

/*
    Port scanner versione basic:
    Logica: Creo un  socket per ogni porta ma lo immposto in modalità non bloccante, in modo da evitare tempi infiniti.
    Non controllo subito il loro output perchè una volta lanciati aspetteranno la risposta in backgroud.
    Questi fd li aggiungo ad un set, che monitorono con select().
    Select() controlla il valore di ritorno di ciascun socket in lista e agisce solo su quelli che hanno risposto
*/

// Variabili globali



int main()
{
    
    int port = 80;

    

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) { perror("fd: "); return FAIL;}

    
    sockaddr_in target{};
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &target.sin_addr);

    // non blocking socket setting
    int flags = fcntl(fd, F_GETFL, 0);    // Current socket flags
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);   // Take all the current and set them to non-blocking 


    int res = connect(fd, (struct sockaddr *)&target, sizeof(target));
    if (errno == EINPROGRESS)
    {
        std::cout << "Connessione in corso - non interrompere.." << std::endl;
    }else{
        std::cout << "Errore immediato - " << strerror(errno) << std::endl;
    }
    

    std::cout << "Connect lanciato sulla porta " << port << std::endl;

    // create a fd set
    fd_set set_list;
    FD_ZERO(&set_list); // Clean the set before usage
    FD_SET(fd, &set_list);  // Add our fd to the set
    
    // time-out section 
    struct timeval timeout;  // Usage of a struct due to historical reason, epoll() use int by default
    timeout.tv_sec = 2;  
    timeout.tv_usec = 0;

    /*
    Notes:

    Select(int __nfds, fd_set *__restrict__ __readfds, fd_set *__restrict__ __writefds, fd_set *__restrict__ __exceptfds, timeval *__restrict__ __timeout):

    - nfds significa "numero di file descriptor". Non è il numero di fd che stai monitorando — è il valore massimo + 1.
    - readfds — l'insieme dei fd da monitorare per lettura. NULL significa "non monitoro per lettura".
    - writefds — l'insieme dei fd da monitorare per scrittura. Passiamo il puntatore al nostro set.
    - exceptfds — eccezzioni (errori urgenti). NULL perché non ci servono.
    - Puntatore a struct con i secondi e i millisecodi 

    Return values: 
    -1  → error (check errno)
    0   → timeout espired
    >0  → number of  fd ready
    */
    int sel = select(fd + 1, NULL, &set_list,NULL, &timeout);

    if (sel == -1)
    {
        perror("Select: ");
        return FAIL;
    }else if (sel == 0)
    {
        std::cout << "Timeout - porta " << port << " Non risponde entro il tempo limite" << std::endl;
    }
    else{
        if (FD_ISSET(fd, &set_list))
        {
            std::cout << "Socket pronto - Controllando il risultato..." << std::endl;
            // check the connection 
            int error;
            socklen_t len = sizeof(error);
            /*
            Notes: 

            getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) - get socket option 

            - sockfd: Il descrittore (file descriptor o handle) del socket da analizzare.
            - level: Il livello protocollare dell'opzione (es. SOL_SOCKET per opzioni generali del socket, 
            - IPPROTO_TCP per opzioni specifiche di TCP).
            - optname: Il nome dell'opzione di cui si vuole ottenere il valore (es. SO_RCVBUF, SO_ERROR).
            - optval: Puntatore a un buffer vuoto in cui la funzione scriverà il valore richiesto.
            - optlen: Puntatore a una variabile che inizialmente deve contenere la dimensione massima del buffer optval. Al ritorno della funzione, conterrà la dimensione reale del valore scritto.
            
            return values:
            - 0 ok
            - -1 error
            */
            getsockopt(fd,SOL_SOCKET, SO_ERROR, &error, &len);

            if (error == 0)
            {
                std::cout << "Porta " << port << " - APERTA " << std::endl;
            }
            else{
                std::cout << "Porta " << port << " - CHIUSA (errore " << error << ")" << std::endl;
            }
            
        }   
    }
    close(fd);
    return OK;
}