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
    
    USER_ACCOUNT accounts[CLIENT_MAX_ACCOUNT_COUNT];
    int accounts_count = 0;                             // najvyssi platny index v poli accounts
    pthread_mutex_t accounts_mutex;
    pthread_mutex_init(&accounts_mutex, NULL);

    // Skontrolujeme či máme dostatok argumentov.
    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    // Vynulujeme a zinicializujeme sieťovú adresu.
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
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

    
    // TODO toto bude fungovat iba pre prvych n klientov, potom server skonci
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
        
        // Ulozenie noveho socketu
        client_sockets[i] = newsockfd;
        
        CLIENT_SOCKET sockData = {
            newsockfd,
            accounts,
            accounts_count,
            &accounts_mutex
        };
        
        // Vytvorenie vlakna pre obsluhu klienta
        pthread_create(&client_threads[i], NULL, clientHandler, &sockData);
        printf("Client %d. connected.\n", i + 1);   
    }
    
    
    // Join vlakien
    for(int i = 0; i < CLIENT_INITIAL_COUNT; i++) {
        pthread_join(client_threads[i], NULL);
    }
    
    printf("Server stopped.\n\n");

    // Zatvorenie klientskych socketov
    for(int i = 0; i < CLIENT_INITIAL_COUNT; i++) {
        close(client_sockets[i]);
    }
    close(sockfd);

    return 0;
}
