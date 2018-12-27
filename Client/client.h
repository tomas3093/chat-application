#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../routines.h"


/* Vytvori hlavicku spravy
 * buffer - premenna do ktorej sa vytvori hlavicka
 * messageCode - kod spravy pre ktoru sa ma vytvorit hlavicka
 * return - stav s akym funkcia skoncila
*/
int addMessageCode(char* buffer, const int messageCode)
{
    char* code = getStrMessageCode(messageCode);
    
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);
    strcat(buffer, code);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    free(code);

    return 0;
}


/* Pripojenie na server
 * sockfd - inicializovany a pripojeny socket
 * buffer - premenna v ktorej sa vrati odpoved servera
 * return - stav s akym funkcia skoncila
*/
int connectToServer(int* sockfd, char* buffer)
{
    addMessageCode(buffer, SOCK_REQ_CONNECT);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       perror("Error writing to socket");
       return 5;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_MESSAGE_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }

    return 0;
}


/* Odpojenie zo servera
 * sockfd - inicializovany a pripojeny socket
 * buffer - premenna v ktorej sa vrati odpoved servera
 * return - stav s akym funkcia skoncila
*/
int disconnectFromServer(int* sockfd, char* buffer)
{
    addMessageCode(buffer, SOCK_REQ_DISCONNECT);
    
    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       perror("Error writing to socket");
       return 5;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_MESSAGE_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }

    return 0;
}


/* Poziadavka na vytvorenie uzivatelskeho konta
 * sockfd - inicializovany a pripojeny socket
 * buffer - premenna v ktorej sa vrati odpoved servera
 * credentials - registracne udaje
 * return - stav s akym funkcia skoncila
*/
int registerMe(int* sockfd, char* buffer, ACCOUNT_CREDENTIALS* credentials)
{
    addMessageCode(buffer, SOCK_REQ_REGISTER);
    
    strcat(buffer, credentials->username);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, credentials->password);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       perror("Error writing to socket");
       return 5;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_MESSAGE_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }

    return 0;
}


/* Poziadavka na prihlasenie uzivatela do jeho konta
 * sockfd - inicializovany a pripojeny socket
 * buffer - premenna v ktorej sa vrati odpoved servera
 * credentials - prihlasovacie udaje
 * username - smernik na miesto kde sa ulozi meno uzivatela po uspesnom prihlaseni
 * return - stav s akym funkcia skoncila
*/
int login(int* sockfd, char* buffer, ACCOUNT_CREDENTIALS* credentials, char* username)
{
    addMessageCode(buffer, SOCK_REQ_LOGIN);
    
    strcat(buffer, credentials->username);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, credentials->password);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       perror("Error writing to socket");
       return 5;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_MESSAGE_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
    
    *username = *(credentials->username);

    return 0;
}


/**
 * Zobrazi uvodne menu aplikacie
 */
void showStartMenu(int* sockfd, char* buffer) {
    printf("\n### Chat app ###\n\n");
    printf("Menu:\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n\n");
    
    int option;
    scanf("%d", &option);
    
    char* value1 = malloc(30 * sizeof(char));
    char* value2 = malloc(30 * sizeof(char));
    
    switch(option) {
        case 1:
            printf("Enter your login:\t");
            scanf("%s", value1);
            printf("\nEnter your password:\t");
            scanf("%s", value2);
            
            printf("\nLogin: %s\nPassword: %s\n\n", value1, value2);
            break;
            
        case 2:
            printf("Enter your login:\t");
            scanf("%s", value1);
            printf("\nEnter your password:\t");
            scanf("%s", value2);
            
            printf("\nLogin: %s\nPassword: %s\n\n", value1, value2);
            
            ACCOUNT_CREDENTIALS c = {value1, value2};
            registerMe(sockfd, buffer, &c);
            break;
            
        case 3:
            break;
            
        default:
            break;
    }
    
    free(value1);
    free(value2);
}


/**
 * Zobrazi menu pre prihlaseneho usera
 */
void showMenuAuthenticated() {
    
}