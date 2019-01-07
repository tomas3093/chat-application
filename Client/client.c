#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../konstanty.h"
#include "client.h"


int main(int argc, char *argv[])
{
    int sockfd;                         // socket pre spojenie so serverom
    struct sockaddr_in serv_addr;       
    struct hostent* server;

    // Buffer sprav, prostrednictvom ktorych prebieha komunikacia
    char buffer[SOCK_BUFFER_LENGTH];    
    
    // Meno aktualne prihlaseneho usera (nastavi sa po prihlaseni)
    char* username = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
    
    
    // Skontrolujeme či máme dostatok argumentov.
    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        free(username);
        return 1;
    }

    // Použijeme funkciu gethostbyname na získanie informácii o počítači, ktorého hostname je v prvom argumente.
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        free(username);
        return 2;
    }

    // Vynulujeme a zinicializujeme adresu, na ktorú sa budeme pripájať.
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
        (char*)server->h_addr_list[0],
        (char*)&serv_addr.sin_addr.s_addr,
        server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    // Vytvoríme si socket v doméne AF_INET.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        free(username);
        return 3;
    }
    puts("Socket created");

    // Pripojíme sa na zadanú sieťovú adresu.
    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        free(username);
        return 4;
    }
    puts("Socket connected\n");

    
    int status = CLIENT_STATUS_UNAUTHENTICATED;
    while (status != CLIENT_STATUS_EXIT) {
        // Registracia alebo prihlasenie
        while (status != CLIENT_STATUS_AUTHENTICATED) {

            // Zobrazenie menu s moznostami prihlasit sa alebo registrovat
            status = showStartMenu(&sockfd, buffer, username);

            // Koniec aplikacie
            if (status == CLIENT_STATUS_EXIT) {
                free(username);
                close(sockfd);
                return 0;
            }
        }

        // Hlavny cyklus s komunikaciou medzi serverom a prihlasenym klientom
        while (status == CLIENT_STATUS_AUTHENTICATED) {

            // Menu pre prihlaseneho uzivatela
            status = showMenuAuthenticated(&sockfd, buffer, username);
            
            // Koniec aplikacie
            if (status == CLIENT_STATUS_EXIT) {
                free(username);
                close(sockfd);
                return 0;
            }
        }
    }

    
    free(username);
    close(sockfd);

    return 0;
}
