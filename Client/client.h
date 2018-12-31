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
    
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
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
    int n = write(*sockfd, buffer, strlen(buffer) + 1);
    if (n < 0)
    {
       perror("Error writing to socket");
       return 5;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
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
    int n = write(*sockfd, buffer, strlen(buffer) + 1);
    if (n < 0)
    {
       perror("Error writing to socket");
       return 5;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
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
    int n = write(*sockfd, buffer, strlen(buffer) + 1);
    if (n < 0)
    {
       perror("Error writing to socket");
       return 5;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
    
    // Zistenie s akym stavom prebehla registracia
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
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
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
    
    *username = *(credentials->username);

    return 0;
}


/**
 * Zobrazi uvodne menu aplikacie (neprihlaseny uzivatel)
 * @param sockfd
 * @param buffer
 * @param noExit - adresa kam sa ulozi priznak ci uzivatel ukoncil aplikaciu (0 - koniec, 1 - pokracuje sa)
 * @param username - adresa kam sa ulozi meno prihlaseneho uzivatela
 */
void showStartMenu(int* sockfd, char* buffer, int* noExit, char* username) {
    printf("\n### Chat app ###\n\n");
    printf("Menu:\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n\n");
    
    int option;
    scanf("%d", &option);
    
    char* username_input = malloc(USER_USERNAME_MAX_LENGTH * sizeof(char));
    char* password_input = malloc(USER_PASSWORD_MAX_LENGTH * sizeof(char));
    int value;
    
    switch(option) {
        case 1:
            
            //TODO Cyklus 3 krat zadanie mena a hesla, ak zada zle tak skonci breakom a noExit = 0
            
            printf("Enter your login:\t");
            scanf("%s", username_input);
            printf("\nEnter your password:\t");
            scanf("%s", password_input);
            
            printf("\nLogin: %s\nPassword: %s\n\n", username_input, password_input);
            
            printf("NOT IMPLEMENTED YET!\n");
            *noExit = 1;
            break;
            
        case 2:
            printf("Enter your login:\t");
            scanf("%s", username_input);
            printf("\nEnter your password:\t");
            scanf("%s", password_input);
            
            ACCOUNT_CREDENTIALS c = {username_input, password_input};
            value = registerMe(sockfd, buffer, &c);
            if (value == SOCK_RES_REGISTER_OK) {
                printf("\nRegistration successful!\n");
                sleep(2);
                *noExit = 1;
                break;
            } 
            else {
                // Chyba pri registracii
                printf("\nRegistration failed! Entered username is not available.\n");
                sleep(2);
                *noExit = 0;
                break;
            }
            
        default:
            printf("Bye!\n");
            *noExit = 0;
            break;
    }
    
    free(username_input);
    free(password_input);
}


/**
 * Zobrazi menu pre prihlaseneho uzivatela
 * @param sockfd
 * @param buffer
 * @param noExit - adresa kam sa ulozi priznak ci uzivatel ukoncil aplikaciu (0 - koniec, 1 - pokracuje sa)
 * @param username - meno prihlaseneho uzivatela
 */
void showMenuAuthenticated(int* sockfd, char* buffer, int* noExit, char* username) {
    system("clear");
    printf("\n### Chat app ###\n\n");
    printf("You are logged as %s\n", username);
    printf("Menu:\n");
    printf("1. Send message\n");
    printf("2. Add contact\n");
    printf("3. Delete contact\n");
    printf("4. Exit\n\n");
    
    int option;
    scanf("%d", &option);
    
    char* value1 = malloc(30 * sizeof(char));
    char* value2 = malloc(30 * sizeof(char));
    
    switch(option) {
        case 1:            
            printf("NOT IMPLEMENTED YET!\n");
            *noExit = 1;
            break;
            
        case 2:
            printf("NOT IMPLEMENTED YET!\n");
            *noExit = 1;
            break;
            
        case 3:
            printf("NOT IMPLEMENTED YET!\n");
            *noExit = 1;
            break;
            
        default:
            printf("Bye!\n");
            *noExit = 0;
            break;
    }
    
    free(value1);
    free(value2);
}