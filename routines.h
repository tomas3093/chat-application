// POMOCNE FUNKCIE

#ifndef _ROUTINES_INCLUDED
#define _ROUTINES_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <bits/stdio2.h>
#include <bits/string_fortified.h>

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


/**
 * Rozparsuje buffer, ktori obsahuje prihlasovacie udaje
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


#endif
