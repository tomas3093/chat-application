#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <c++/8/bits/ios_base.h>

#include "../konstanty.h"


/* Vytvori hlavicku spravy
 * buffer - premenna do ktorej sa vytvori hlavicka
 * messageCode - kod spravy pre ktoru sa ma vytvorit hlavicka
 * return - stav s akym funkcia skoncila
*/
int addMessageCode(char* buffer, const int messageCode)
{
    memset(buffer, 0, SOCK_MESSAGE_BUFFER_LENGTH);
    strcat(buffer, getStrMessageCode(messageCode));
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);

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


/**
 * Zobrazi uvodne menu aplikacie
 */
void showStartMenu() {
    printf("\n### Chat app ###\n\n");
    printf("Menu:\n");
    printf("1. Login\n");
    printf("2. Sign up\n");
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
            
            printf("\nLogin: %s\nPassword: %s\n", value1, value2);
            break;
            
        case 2:
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