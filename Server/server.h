#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>

#include "../konstanty.h"
#include "../routines.h"


/**
 * Obsluha poziadavky pre pripojenie klienta
 * @param p
 */
void clientConnectHandler(CLIENT_SOCKET* p) {
    printf("Klient sa pripojil\n");

    // Pošleme odpoveď klientovi.
    char* buffer = malloc(SOCK_BUFFER_LENGTH);
    memset(buffer, 0, SOCK_BUFFER_LENGTH);

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
void clientDisconnectHandler(CLIENT_SOCKET* p) {
    printf("Klient sa odpojil\n");

    // Pošleme odpoveď klientovi.
    char* buffer = malloc(SOCK_BUFFER_LENGTH);
    memset(buffer, 0, SOCK_BUFFER_LENGTH);

    char* messageCode = getStrMessageCode(SOCK_RES_DISCONNECT);
    strcat(buffer, messageCode);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, "Server prijal ziadost o disconnect.\n\nDovidenia.");
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
 * @param buffer - buffer pre spravu klientovi
 * @param credentials - udaje noveho uzivatela
 */
void clientRegisterHandler(CLIENT_SOCKET* p, char* buffer, ACCOUNT_CREDENTIALS* credentials) {
    int registration_successful;    // priznak ci prebehla registracia uspesne
    char* messageText = malloc(sizeof(char) * SOCK_MESSAGE_LENGTH);  // odpoved
    
    // Validita zadanych udajov
    if (    strlen(credentials->username) >= USER_USERNAME_MIN_LENGTH && 
            strlen(credentials->username) <= USER_USERNAME_MAX_LENGTH &&
            strlen(credentials->password) >= USER_PASSWORD_MIN_LENGTH &&
            strlen(credentials->password) <= USER_PASSWORD_MAX_LENGTH) {

        // Vytvorenie struktury
        USER_ACCOUNT* account = malloc(sizeof(USER_ACCOUNT));
        account->credentials = credentials;
        account->active = 1;
        
        // Kontrola ci je zadane meno unikatne
        int isUnique = 1;
        pthread_mutex_lock(p->accounts_mutex);
        for(int i = 0; i < p->accounts_count; i++) {
            if (strcmp(p->accounts[i].credentials->username, credentials->username) == 0) {
                isUnique = 0;
                break;
            }
        }
        
        // Je unikatne a je volne miesto
        if (isUnique == 1 && p->accounts_count < CLIENT_MAX_ACCOUNT_COUNT) {
            
            // Registracia
            p->accounts[p->accounts_count] = *account;
            p->accounts_count++;
            registration_successful = 1;
            messageText = "Registracia bola uspesna.";
        } else {
            free(account);
            registration_successful = 0;
            messageText = "Registracia zlyhala. Nie je dostatocna velkost pamate.";
        }
        pthread_mutex_unlock(p->accounts_mutex);
        
    } else {
        registration_successful = 0;
        messageText = "Registracia zlyhala. Meno alebo heslo nema spravnu dlzku.";
    }

    // Odpoved klientovi
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    
    char* messageCode;
    messageCode = registration_successful == 1 ? 
        getStrMessageCode(SOCK_RES_REGISTER_OK) : getStrMessageCode(SOCK_RES_REGISTER_FAIL);
    
    
    strcat(buffer, messageCode);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, messageText);
    
    int n = write(p->newsockfd, buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
    
    free(messageCode);
    //free(messageText);
}


/**
 * Obsluha poziadavky pre prihlasenie klienta
 * @param p
 * @param buffer
 * @param credentials
 */
void clientLoginHandler(CLIENT_SOCKET* p, char* buffer, ACCOUNT_CREDENTIALS* credentials) {
    // TODO
}

/**
 * Obsluha daneho klienta v jeho vlakne
 * @return 
 */
void* clientHandler(void* args) {
    
    CLIENT_SOCKET* p = (CLIENT_SOCKET*)args;
    
    char buffer[SOCK_BUFFER_LENGTH];   // buffer pre spravy odosielane medzi serverom a klientom
    
    // Hlavny cyklus s komunikaciou medzi serverom a klientom
    int isEnd = 0;          // podmienka ukoncenia cyklu
    while (isEnd == 0) {

        // Načítame správu od klienta cez socket do buffra.
        memset(buffer, 0, SOCK_BUFFER_LENGTH);
        int n = read(p->newsockfd, buffer, SOCK_BUFFER_LENGTH - 1);
        if (n < 0)
        {
            perror("Error reading from socket");
            return NULL;
        }
        
        // Zistenie typu spravy a zavolanie danej obsluhy
        int messageCode = getMessageCode(buffer);
        if (messageCode == SOCK_REQ_CONNECT) {
            clientConnectHandler(p);
        } else if (messageCode == SOCK_REQ_DISCONNECT) {
            clientDisconnectHandler(p);
            isEnd = 1;
        } else if (messageCode == SOCK_REQ_REGISTER) {
            ACCOUNT_CREDENTIALS* credentials = getCredentialsFromBuffer(buffer);
            clientRegisterHandler(p, buffer, credentials);
        } else {
            // Do nothing
        }
    }
    
    close(p->newsockfd);
      
    return NULL;
}