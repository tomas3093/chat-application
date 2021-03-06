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
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    
    // Uzivatelske ucty
    USER_ACCOUNT* accounts[CLIENT_MAX_ACCOUNT_COUNT];
    int accounts_count = 0;                             // najvyssi platny index v poli accounts
    pthread_mutex_t accounts_mutex;
    pthread_mutex_init(&accounts_mutex, NULL);
    
    // Nedorucene spravy
    MESSAGE* messages[SERVER_MAX_MESSAGES_COUNT];
    int messages_count = 0;
    pthread_mutex_t messages_mutex;
    pthread_mutex_init(&messages_mutex, NULL);
    
    
    if (DEBUG_MODE == 1) {
        puts("Debug mode enabled");
        ACCOUNT_CREDENTIALS* c1 = malloc(sizeof(ACCOUNT_CREDENTIALS));
        c1->username = "tom1";
        c1->password = "123456";
        
        ACCOUNT_CREDENTIALS* c2 = malloc(sizeof(ACCOUNT_CREDENTIALS));
        c2->username = "tom2";
        c2->password = "123456";
        
        USER_ACCOUNT* a1 = malloc(sizeof(USER_ACCOUNT));
        a1->contacts = malloc(sizeof(int*) * CLIENT_MAX_ACCOUNT_COUNT);
        a1->active = malloc(sizeof(int));
        a1->credentials = c1;
        *a1->active = 1;
        // Inicializacia pola kontaktov
        for(int i = 0; i < CLIENT_MAX_ACCOUNT_COUNT; i++) {
            a1->contacts[i] = malloc(sizeof(int));
            *a1->contacts[i] = 0;
        }
        
        USER_ACCOUNT* a2 = malloc(sizeof(USER_ACCOUNT));
        a2->contacts = malloc(sizeof(int*) * CLIENT_MAX_ACCOUNT_COUNT);
        a2->active = malloc(sizeof(int));
        a2->credentials = c2;
        *a2->active = 1;
        // Inicializacia pola kontaktov
        for(int i = 0; i < CLIENT_MAX_ACCOUNT_COUNT; i++) {
            a2->contacts[i] = malloc(sizeof(int));
            *a2->contacts[i] = 0;
        }
        
        accounts[0] = a1;
        accounts[1] = a2;
        accounts_count = 2;
        
        *a1->contacts[1] = 1;
        *a2->contacts[0] = 1;
        
        MESSAGE* m1 = malloc(sizeof(MESSAGE));
        m1->unread = malloc(sizeof(int));
        m1->sender = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
        m1->recipient = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
        m1->text = malloc(sizeof(char) * CLIENT_MESSAGE_LENGTH);
        *m1->unread = 1;
        m1->sender = "tom1";
        m1->recipient = "tom2";
        m1->text = "hello!";
        
        MESSAGE* m2 = malloc(sizeof(MESSAGE));
        m2->unread = malloc(sizeof(int));
        m2->sender = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
        m2->recipient = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
        m2->text = malloc(sizeof(char) * CLIENT_MESSAGE_LENGTH);
        *m2->unread = 1;
        m2->sender = "tom1";
        m2->recipient = "tom2";
        m2->text = "this is my second message!";
        
        messages[0] = m1;
        messages[1] = m2;
        messages_count = 2;
    }

    
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
        int* new_sock = malloc(1);
        *new_sock = client_sock;
        
        CLIENT_SOCKET* sockData = malloc(sizeof(CLIENT_SOCKET));
        sockData->client_sock = new_sock;
        sockData->accounts = accounts;
        sockData->accounts_count = &accounts_count;
        sockData->accounts_mutex = &accounts_mutex;
        
        sockData->messages = messages;
        sockData->messages_count = &messages_count;
        sockData->messages_mutex = &messages_mutex;
        
        if (pthread_create(&sniffer_thread, NULL, clientHandler, sockData) < 0) {
            perror("Could not create thread");
            return 1;
        }
        
        //pthread_join(sniffer_thread, NULL);
        puts("Handler assigned");   
    }
    
    // Uvolnenie pamete
    pthread_mutex_destroy(&accounts_mutex);
    pthread_mutex_destroy(&messages_mutex);
    
    // Uzivatelia
    for (int i = 0; i < accounts_count; i++) {
        free(accounts[i]->credentials->username);
        free(accounts[i]->credentials->password);
        free(accounts[i]->credentials);
        free(accounts[i]->active);
        for(int j = 0; j < CLIENT_MAX_ACCOUNT_COUNT; j++) {
            free(accounts[i]->contacts[j]);
        }
        free(accounts[i]->contacts);
    }
    
    // Spravy
    for (int i = 0; i < messages_count; i++) {
        free(messages[i]->unread);
        free(messages[i]->sender);
        free(messages[i]->recipient);
        free(messages[i]->text);
    }
    
    // sockData ?
    
    
    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}
