// POMOCNE FUNKCIE

#ifndef _ROUTINES_INCLUDED
#define _ROUTINES_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "konstanty.h"


/**
 * Vrati ciselny kod typu zo zadanej spravy
 * @param messageBuffer
 * @return 
 */
int getMessageCode(const char* messageBuffer) {
    char number[SOCK_MESSAGE_CODE_MAX_LENGTH];
    memset(number, 0, SOCK_MESSAGE_CODE_MAX_LENGTH);
    
    int i;
    for(i = 0; i < SOCK_MESSAGE_CODE_MAX_LENGTH; i++) {
        if (messageBuffer[i] != SOCK_SPECIAL_SYMBOL) {
            number[i] = messageBuffer[i];
        } else {
            break;
        }        
    }
    
    if (i > 0) {
        return strtol(number, NULL, 0);
    } else {
        return -1;
    }
}


/**
 * Vrati ciselny kod spravy ako retazec
 * @param code
 * @return 
 */
char* getStrMessageCode(int code) {
    char* messageCode = malloc(sizeof(char) * SOCK_MESSAGE_CODE_MAX_LENGTH);
    sprintf(messageCode, "%d", code);
    
    return messageCode;
}


/* Vytvori hlavicku spravy
 * buffer - premenna do ktorej sa vytvori hlavicka
 * messageCode - kod spravy pre ktoru sa ma vytvorit hlavicka
 * return - stav s akym funkcia skoncila
*/
int addMessageCode(char* buffer, const int messageCode)
{
    char* code = getStrMessageCode(messageCode);
    
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    strcat(buffer, code);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    free(code);

    return 0;
}


/**
 * Rozparsuje buffer, ktori obsahuje prihlasovacie udaje (prvy a druhy argument)
 * @return 
 */
ACCOUNT_CREDENTIALS* getCredentialsFromBuffer(char* buffer) {
    ACCOUNT_CREDENTIALS* credentials = malloc(sizeof(ACCOUNT_CREDENTIALS));
    credentials->username = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
    credentials->password = malloc(sizeof(char) * USER_PASSWORD_MAX_LENGTH);
    memset(credentials->username, 0, USER_USERNAME_MAX_LENGTH);
    memset(credentials->password, 0, USER_PASSWORD_MAX_LENGTH);
    
    int first = 0;
    while(1) {
        if (buffer[first] == '\0' || buffer[first] == SOCK_SPECIAL_SYMBOL) {
            break;
        }
        
        first++;
    }
    
    int second = first + 1;
    while(1) {
        if (buffer[first] == '\0' || buffer[second] == '\0' || buffer[second] == SOCK_SPECIAL_SYMBOL) {
            break;
        }
        
        second++;
    }
    
    memcpy(credentials->username, buffer + (first + 1), second - first - 1);
    memcpy(credentials->password, buffer + (second + 1), strlen(buffer) - second - 1);
    
    return credentials;
}


/**
 * Vrati druhy argument z buffera (argument za prvym specialnym symbolom)
 * @param buffer
 * @return 
 */
char* getSecondBufferArgument(char* buffer) {
    char* arg = malloc(sizeof(char) * SOCK_BUFFER_LENGTH);
    memset(arg, 0, SOCK_BUFFER_LENGTH);
    
    int first = 0;
    while(1) {
        if (buffer[first] == '\0' || buffer[first] == SOCK_SPECIAL_SYMBOL) {
            break;
        }
        
        first++;
    }
    
    int second = first + 1;
    while(1) {
        if (buffer[first] == '\0' || buffer[second] == '\0' || buffer[second] == SOCK_SPECIAL_SYMBOL) {
            break;
        }
        
        second++;
    }
    
    memcpy(arg, buffer + (first + 1), second - first - 1);
    
    return arg;
}


/**
 * Vrati treti argument z buffera (argument za druhym specialnym symbolom)
 * @param buffer
 * @return 
 */
char* getThirdBufferArgument(char* buffer) {
    char* arg = malloc(sizeof(char) * SOCK_BUFFER_LENGTH);
    memset(arg, 0, SOCK_BUFFER_LENGTH);
    
    int first = 0;
    while(1) {
        if (buffer[first] == '\0' || buffer[first] == SOCK_SPECIAL_SYMBOL) {
            break;
        }
        
        first++;
    }
    
    int second = first + 1;
    while(1) {
        if (buffer[first] == '\0' || buffer[second] == '\0' || buffer[second] == SOCK_SPECIAL_SYMBOL) {
            break;
        }
        
        second++;
    }
    
    memcpy(arg, buffer + (second + 1), strlen(buffer) - second - 1);
    
    return arg;
}


/**
 * Vytvori v bufferi chybovu spravu
 * @param buffer - miesto kde sa ma sprava vytvorit
 * @param messageText - chybova hlaska
 * @return 
 */
int newErrorMessage(char* buffer, const char* messageText) {
    addMessageCode(buffer, SOCK_RES_FAIL);
    strcat(buffer, messageText);
    
    return SOCK_RES_FAIL;
}


/**
 * Odosle na socket odpoved s kodom (Uspech alebo neuspech)
 * @param p
 * @param buffer
 * @param statusCode
 */
void sendResponseStatus(CLIENT_SOCKET* p, char* buffer, int statusCode) {
    // Odpoved
    addMessageCode(buffer, statusCode);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        newErrorMessage(buffer, "Error writing to socket");
    }
}


/**
 * Vypise chybovu hlasku z buffra na standardny vystup
 * @param buffer
 */
void printErrorMessage(char* buffer) {
    char* msg = getSecondBufferArgument(buffer);
    puts(msg);
    free(msg);
}


#endif
