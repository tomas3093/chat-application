#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include "../konstanty.h"


/**
 * Obsluha daneho klienta v jeho vlakne
 * @return 
 */
void* clientHandler(void* args) {
    
    CLIENT_SOCKET* p = (CLIENT_SOCKET*)args;
    
    char buffer[256];   // buffer pre spravy odosielane medzi serverom a klientom
    int n;              // premenna, pomocou ktorej sa zistuje uspech citania/zapisovania do socketu
    
    // Hlavny cyklus s komunikaciou medzi serverom a klientom
    while (1) {

        // Načítame správu od klienta cez socket do buffra.
        memset(buffer, 0, 256);
        n = read(p->newsockfd, buffer, 255);
        if (n < 0)
        {
            perror("Error reading from socket");
            //return 4;
            return NULL;
        }

        // TODO Tieto IF-y dat do switchu a funkcionalitu dat do funkcii
        // v subore server.h (este nie je vytvoreny)
        if (buffer[0] == *SOCK_REQ_CONNECT) {
            printf("Klient sa pripojil\n");

            // Pošleme odpoveď klientovi.
            char* msg = malloc(50 * sizeof(char));
            memset(msg, 0, 50);

            strcat(msg, SOCK_RES_CONNECT);
            strcat(msg, &SOCK_SPECIAL_SYMBOL);
            strcat(msg, "Server prijal ziadost o connect.");
            n = write(p->newsockfd, msg, strlen(msg)+1);
            free(msg);

            if (n < 0)
            {
                perror("Error writing to socket");
                //return 5;
                return NULL;
            }
        }

        if (buffer[0] == *SOCK_REQ_DISCONNECT) {
            printf("Klient sa odpojil\n");

            // Pošleme odpoveď klientovi.
            char* msg = malloc(50 * sizeof(char));
            memset(msg, 0, 50);

            strcat(msg, SOCK_RES_DISCONNECT);
            strcat(msg, &SOCK_SPECIAL_SYMBOL);
            strcat(msg, "Server prijal ziadost o disconnect.\n\nDovidenia.\n");
            n = write(p->newsockfd, msg, strlen(msg)+1);
            free(msg);

            if (n < 0)
            {
                perror("Error writing to socket");
                //return 5;
                return NULL;
            }
        }
    }
    
    close(p->newsockfd);
      
    return NULL;
    //return 0;
}