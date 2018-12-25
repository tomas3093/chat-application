/*
 * File:   server.c
 * Author: blazy
 *
 * Created on November 24, 2018, 12:09 PM
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "server.h"


int main(int argc, char *argv[])
{
    int sockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    
    int client_sockets[CLIENT_INITIAL_COUNT];
    pthread_t client_threads[CLIENT_INITIAL_COUNT];

    // Skontrolujeme či máme dostatok argumentov.
    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    // Vynulujeme a zinicializujeme sieťovú adresu.
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    // Vytvoríme si socket v doméne AF_INET.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    // Priradíme vyplnenú sieťovú adresu vytvorenému socketu.
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    // Pripravíme socket pre príjmanie spojení od klientov. Maximálna dĺžka fronty neobslúžených spojení je 5.
    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);

    printf("Server started.\n\n");

    
    // Obsluha klientov
    for(int i = 0; i < CLIENT_INITIAL_COUNT; i++) {
        
        // Počkáme na a príjmeme spojenie od klienta.
        int newsockfd;
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            return 3;
        }
        
        CLIENT_SOCKET clientSock = {
            newsockfd
        };
        
        pthread_create(&client_threads[i], NULL, clientHandler, &clientSock);
        printf("Client %d. connected.\n", i);
    }
    
    
    // Join vlakien
    for(int i = 0; i < CLIENT_INITIAL_COUNT; i++) {
        pthread_join(client_threads[i], NULL);
    }
    
    printf("Server stopped.\n\n");

    close(sockfd);

    return 0;
}
