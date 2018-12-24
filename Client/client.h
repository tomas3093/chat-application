#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "konstanty.h"


/* Vytvori hlavicku spravy
 * buffer - premenna do ktorej sa vytvori hlavicka
 * messageCode - kod spravy pre ktoru sa ma vytvorit hlavicka
 * return - stav s akym funkcia skoncila
*/
int addMessageCode(char* buffer, const char* messageCode)
{
    bzero(buffer,SOCK_BUFFER_LENGTH);
    strcat(buffer, messageCode);
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
    bzero(buffer,256);
    n = read(*sockfd, buffer, 255);
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
    bzero(buffer,256);
    n = read(*sockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }

    return 0;
}
