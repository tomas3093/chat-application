// POMOCNE FUNKCIE

#ifndef _ROUTINES_INCLUDED
#define _ROUTINES_INCLUDED

#include <string.h>
#include <stdlib.h>

#include "konstanty.h"


/**
 * Vrati ciselny kod zadanej spravy
 * @param messageBuffer
 * @return 
 */
int getMessageCode(const char* messageBuffer) {
    char number[SOCK_MESSAGE_CODE_MAX_LENGTH];
    memset(number, 0, SOCK_MESSAGE_CODE_MAX_LENGTH);
    
    int i;
    for(i = 0; i < SOCK_MESSAGE_CODE_MAX_LENGTH; i++) {
        if (messageBuffer[i] != SOCK_SPECIAL_SYMBOL) {
            strcat(&number, &messageBuffer[i]);
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
 * Vrati kod zadanej spravy ako retazec
 * @param messageBuffer
 * @return 
 */
char* getStrMessageCode(int code) {
    char* messageCode = malloc(sizeof(char) * SOCK_MESSAGE_CODE_MAX_LENGTH);
    sprintf(messageCode, "%d", code);
    
    return messageCode;
}


#endif
