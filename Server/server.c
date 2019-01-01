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
    int sockfd;         // Server socket descriptor
    int client_sock;
    int* new_sock;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    
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
    puts("Socket created.");

    // Priradíme vyplnenú sieťovú adresu vytvorenému socketu.
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }
    puts("Bind done.");

    // Pripravíme socket pre príjmanie spojení od klientov. Maximálna dĺžka fronty neobslúžených spojení je 5.
    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);

    printf("Server listening at 127.0.0.1:%s\n\n", argv[1]);
    puts("Waiting for incomming connections...");

    
    // Obsluha klientov
    while(client_sock = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len)) {
        
        puts("Connection accepted");
        
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
        
        CLIENT_SOCKET* sockData = malloc(sizeof(CLIENT_SOCKET));
        sockData->client_sock = new_sock;
        sockData->accounts = accounts;
        sockData->accounts_count = accounts_count;
        sockData->accounts_mutex = &accounts_mutex;
        
        if (pthread_create(&sniffer_thread, NULL, clientHandler, sockData) < 0) {
            perror("Could not create thread");
            return 1;
        }
        
        //pthread_join(sniffer_thread, NULL);
        puts("Handler assigned");   
    }
    
    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}
