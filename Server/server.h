#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>

#include "../konstanty.h"
#include "../routines.h"


/**
 * Obsluha poziadavky pre pripojenie klienta
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 */
void clientConnectHandler(CLIENT_SOCKET* p, char* buffer) {

    // Pošleme odpoveď klientovi.
    addMessageCode(buffer, SOCK_REQ_CONNECT);
    strcat(buffer, "Server prijal ziadost o connect.");
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
}


/**
 * Obsluha poziadavky pre odpojenie klienta
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 */
void clientDisconnectHandler(CLIENT_SOCKET* p, char* buffer) {
    
    // Pošleme odpoveď klientovi.
    addMessageCode(buffer, SOCK_RES_DISCONNECT);
    strcat(buffer, "Server prijal ziadost o disconnect.\n\nDovidenia.");
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
}


/**
 * Obsluha poziadavky pre registraciu noveho klienta
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param credentials - udaje noveho uzivatela
 */
void clientRegisterHandler(CLIENT_SOCKET* p, char* buffer, ACCOUNT_CREDENTIALS* credentials) {
    int registration_successful;    // priznak ci prebehla registracia uspesne
    char* messageText;              // odpoved
    
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
    const int messageCode = registration_successful == 1 ? 
        SOCK_RES_REGISTER_OK : SOCK_RES_REGISTER_FAIL;
    
    addMessageCode(buffer, messageCode);
    strcat(buffer, messageText);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
}


/**
 * Obsluha poziadavky pre prihlasenie klienta
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param credentials - udaje uzivatela, ktory sa chce prihlasit
 */
void clientLoginHandler(CLIENT_SOCKET* p, char* buffer, ACCOUNT_CREDENTIALS* credentials) {
    int login_successful;   // priznak ci prebehlo prihlasenie uspesne
    char* messageText;      // odpoved
    
    // Validita zadanych udajov
    if (    strlen(credentials->username) >= USER_USERNAME_MIN_LENGTH && 
            strlen(credentials->username) <= USER_USERNAME_MAX_LENGTH &&
            strlen(credentials->password) >= USER_PASSWORD_MIN_LENGTH &&
            strlen(credentials->password) <= USER_PASSWORD_MAX_LENGTH) {
        
        // Kontrola ci je zadane meno registrovane a je spravne heslo
        int isValid = 0;
        pthread_mutex_lock(p->accounts_mutex);
        for(int i = 0; i < p->accounts_count; i++) {
            if (strcmp(p->accounts[i].credentials->username, credentials->username) == 0) {
                if (strcmp(p->accounts[i].credentials->password, credentials->password) == 0) {
                    isValid = 1;
                    break;
                } else {
                    // Nespravne heslo
                    break;
                }
            }
        }
        
        // Zadane udaje su spravne
        if (isValid == 1) {
            
            // Prihlasenie
            login_successful = 1;
            messageText = "Prihlasenie bolo uspesne.";
        } else {
            login_successful = 0;
            messageText = "Prihlasenie nebolo uspesne. Nespravne meno alebo heslo.";
        }
        pthread_mutex_unlock(p->accounts_mutex);
        
    } else {
        login_successful = 0;
        messageText = "Prihlasenie nebolo uspesne.";
    }

    // Odpoved klientovi    
    const int messageCode = login_successful == 1 ? 
        SOCK_RES_LOGIN_OK : SOCK_RES_LOGIN_FAIL;
    
    addMessageCode(buffer, messageCode);
    strcat(buffer, messageText);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
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
    
    char buffer[SOCK_BUFFER_LENGTH];   // buffer pre spravy odosielane medzi serverom a klientom
    int read_size;
    
    // Hlavny cyklus s komunikaciou medzi serverom a klientom
    // Načítame správu od klienta cez socket do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    while ((read_size = recv(*(p->client_sock), buffer, SOCK_BUFFER_LENGTH - 1, 0)) > 0) {

        // Zistenie typu spravy a zavolanie danej obsluhy
        int messageCode = getMessageCode(buffer);

        if (messageCode == SOCK_REQ_CONNECT) {
            clientConnectHandler(p, buffer);
        } else if (messageCode == SOCK_REQ_DISCONNECT) {
            clientDisconnectHandler(p, buffer);
            break;  // koniec
        } else if (messageCode == SOCK_REQ_REGISTER) {
            ACCOUNT_CREDENTIALS* credentials = getCredentialsFromBuffer(buffer);
            clientRegisterHandler(p, buffer, credentials);
        } else if (messageCode == SOCK_REQ_LOGIN) {
            ACCOUNT_CREDENTIALS* credentials = getCredentialsFromBuffer(buffer);
            clientLoginHandler(p, buffer, credentials);
        } else if (messageCode == SOCK_REQ_LOGOUT) {
            // TODO
        } else {
            // Do nothing
        }
        
        // Vymazanie buffra
        memset(buffer, 0, SOCK_BUFFER_LENGTH);
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if (read_size < 0) {
        perror("Receive failed");
    }         
    
    free(p->client_sock);
    free(p);
      
    return 0;
}