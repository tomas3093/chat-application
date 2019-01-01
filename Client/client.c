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
    
    // Priznak ci sa ma pokracovat v obsluhe (ukoncenie nekonecneho cyklu)
    int looping;
    
    // Skontrolujeme či máme dostatok argumentov.
    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    // Použijeme funkciu gethostbyname na získanie informácii o počítači, ktorého hostname je v prvom argumente.
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
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
        return 3;
    }

    // Pripojíme sa na zadanú sieťovú adresu.
    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }

    // Poziadavka o pripojenie sa na server
    if (connectToServer(&sockfd, buffer) > 0) {
        // ak zlyhala
        printf("Error during connection to server\n");
        close(sockfd);
        return 5;
    }
    printf("%s\n", buffer); // Vypisanie odpovede servera
    
    // Poziadavka na registraciu alebo prihlasenie
    while (strlen(username) < USER_USERNAME_MIN_LENGTH) {
        
        // Zobrazenie menu s moznostami prihlasit sa alebo registrovat
        int value = showStartMenu(&sockfd, buffer, &looping, username);
        
        // Ak uzivatel chce ukoncit aplikaciu
        if (value >= 3) {
            looping = 0;
            break;
        }

    }

    // Pokracovat do hlavneho cyklu sa bude iba ak je username vyplneny
    
    // Hlavny cyklus s komunikaciou medzi serverom a prihlasenym klientom
    while ( looping == 1 && 
            strlen(username) >= USER_USERNAME_MIN_LENGTH) {

        // Menu pre prihlaseneho uzivatela
        showMenuAuthenticated(&sockfd, buffer, &looping, username);
    }

    free(username);
    //close(sockfd);

    return 0;
}
