#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "konstanty.h"
#include "client.h"


int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char buffer[SOCK_BUFFER_LENGTH];

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
    bzero((char*)&serv_addr, sizeof(serv_addr));
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


    // TODO
    // Poziadavka o pripojenie sa na server
    connectToServer(&sockfd, buffer);
    printf("%s\n",buffer); // Vypisanie odpovede servera

    // Hlavny cyklus s komunikaciou medzi serverom a klientom
    while (1) {

        // TODO
        printf("Stop\n");
        sleep(3);
        disconnectFromServer(&sockfd, buffer);
        break;

        /*
        // Vyzveme používateľa aby zadal text správy pre server.
        printf("Please enter a message: ");

        // Načítame správu od používateľa zo štandardného vstupu do buffra.
        bzero(buffer,256);
        fgets(buffer, 255, stdin);

        // Pošleme správu cez socket servru.
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
        {
            perror("Error writing to socket");
            return 5;
        }*/
    }

    close(sockfd);

    return 0;
}
