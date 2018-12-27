#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "../konstanty.h"
#include "../routines.h"


/**
 * Obsluha poziadavky pre pripojenie klienta
 * @param p
 */
void clientConnect(CLIENT_SOCKET* p) {
    printf("Klient sa pripojil\n");

    // Pošleme odpoveď klientovi.
    char* buffer = malloc(SOCK_MESSAGE_BUFFER_LENGTH);
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);

    char* messageCode = getStrMessageCode(SOCK_REQ_CONNECT);
    strcat(buffer, messageCode);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, "Server prijal ziadost o connect.");
    int n = write(p->newsockfd, buffer, strlen(buffer) + 1);
    free(buffer);
    free(messageCode);

    if (n < 0)
    {
        perror("Error writing to socket");
    }
}


/**
 * Obsluha poziadavky pre odpojenie klienta
 * @param p
 */
void clientDisconnect(CLIENT_SOCKET* p) {
    printf("Klient sa odpojil\n");

    // Pošleme odpoveď klientovi.
    char* buffer = malloc(SOCK_MESSAGE_BUFFER_LENGTH);
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);

    char* messageCode = getStrMessageCode(SOCK_RES_DISCONNECT);
    strcat(buffer, messageCode);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, "Server prijal ziadost o disconnect.\n\nDovidenia.\n");
    int n = write(p->newsockfd, buffer, strlen(buffer) + 1);
    free(buffer);
    free(messageCode);

    if (n < 0)
    {
        perror("Error writing to socket");
    }
}


/**
 * Obsluha poziadavky pre registraciu noveho klienta
 * @param p
 */
void clientRegister(CLIENT_SOCKET* p, ACCOUNT_CREDENTIALS* credentials) {
    printf("Klient poziadal o registraciu\nmeno: %s\nHeslo: %s\n", credentials->username, credentials->password);

    // Pošleme odpoveď klientovi.
    char* buffer = malloc(SOCK_MESSAGE_BUFFER_LENGTH);
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);

    // Vyhodnotit ci zadal dobre udaje
    // TODO
    // prejst pole vsetkych uzivatelov ci uz nahodou dane meno nie je obsadene
    
    char* messageCode = getStrMessageCode(SOCK_RES_REGISTER_OK);
    strcat(buffer, messageCode);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, "Registracia uspesna.\n");
    int n = write(p->newsockfd, buffer, strlen(buffer) + 1);
    free(buffer);
    free(messageCode);

    if (n < 0)
    {
        perror("Error writing to socket");
    }
}


/**
 * Obsluha daneho klienta v jeho vlakne
 * @return 
 */
void* clientHandler(void* args) {
    
    CLIENT_SOCKET* p = (CLIENT_SOCKET*)args;
    
    char buffer[SOCK_MESSAGE_BUFFER_LENGTH];   // buffer pre spravy odosielane medzi serverom a klientom
    
    // Hlavny cyklus s komunikaciou medzi serverom a klientom
    int isEnd = 0;          // podmienka ukoncenia cyklu
    while (isEnd == 0) {

        // Načítame správu od klienta cez socket do buffra.
        memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);
        int n = read(p->newsockfd, buffer, SOCK_MESSAGE_BUFFER_LENGTH - 1);
        if (n < 0)
        {
            perror("Error reading from socket");
            return NULL;
        }
        
        // Zistenie typu spravy a zavolanie danej obsluhy
        int messageCode = getMessageCode(buffer);
        if (messageCode == SOCK_REQ_CONNECT) {
            clientConnect(p);
        } else if (messageCode == SOCK_REQ_DISCONNECT) {
            clientDisconnect(p);
            isEnd = 1;
        } else if (messageCode == SOCK_REQ_REGISTER) {
            clientRegister(p, getCredentialsFromBuffer(buffer));
        } else {
            // Do nothing
        }
    }
    
    close(p->newsockfd);
      
    return NULL;
}