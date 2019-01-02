#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../routines.h"


/* Poziadavka na vytvorenie uzivatelskeho konta
 * sockfd - inicializovany a pripojeny socket
 * buffer - premenna v ktorej sa vrati odpoved servera
 * credentials - registracne udaje
 * return - stav s akym funkcia skoncila
*/
int registerNewClient(int* sockfd, char* buffer, ACCOUNT_CREDENTIALS* credentials)
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
       return SOCK_RES_FAIL;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return SOCK_RES_FAIL;
    }
    
    // Zistenie s akym stavom prebehla registracia
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


/* Poziadavka na prihlasenie uzivatela do jeho konta
 * sockfd - inicializovany a pripojeny socket
 * buffer - premenna v ktorej sa vrati odpoved servera
 * credentials - prihlasovacie udaje
 * return - stav s akym funkcia skoncila
*/
int loginClient(int* sockfd, char* buffer, ACCOUNT_CREDENTIALS* credentials)
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
       return SOCK_RES_FAIL;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return SOCK_RES_FAIL;
    }
    
    // Zistenie s akym stavom prebehlo prihlasenie
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


int showUserContacts(int* sockfd, char* buffer) {
    addMessageCode(buffer, SOCK_REQ_GET_CONTACTS);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       perror("Error writing to socket");
       return SOCK_RES_FAIL;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return SOCK_RES_FAIL;
    }
    
    // Zistenie s akym stavom prebehla poziadavka
    int responseCode = getMessageCode(buffer);
    if (responseCode == SOCK_RES_OK) {
        
        // TODO Naparsovat kontakty z buffra a vypisat ich na konzolu
        puts("NOT IMPLEMENTED YET! - Zobrazenie kontaktov");
    }

    
    return responseCode;
}


/**
 * Prida uzivatela s danym menom medzi kontakty aktualne prihlaseneho uzivatela
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @param contactUsername - meno uzivatela, ktory sa ma pridat
 * @return - stav s akym funkcia skoncila
 */
int addNewContact(int* sockfd, char* buffer, char* contactUsername) {
    addMessageCode(buffer, SOCK_REQ_ADD_CONTACT);
    
    strcat(buffer, contactUsername);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       perror("Error writing to socket");
       return SOCK_RES_FAIL;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return SOCK_RES_FAIL;
    }
    
    // Zistenie s akym stavom prebehlo pridanie medzi kontakty
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


/**
 * Odoberie uzivatela s danym menom z kontaktov aktualne prihlaseneho uzivatela
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @param contactUsername - meno uzivatela, ktory sa ma odobrat
 * @return - stav s akym funkcia skoncila
 */
int deleteContact(int* sockfd, char* buffer, char* contactUsername) {
    addMessageCode(buffer, SOCK_REQ_DELETE_CONTACT);
    
    strcat(buffer, contactUsername);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       perror("Error writing to socket");
       return SOCK_RES_FAIL;
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        perror("Error reading from socket");
        return SOCK_RES_FAIL;
    }
    
    // Zistenie s akym stavom prebehlo odobratie z kontaktov
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


// TODO
int sendMessage(int* sockfd, char* buffer, char* recipient, char* message) {
    
}


/**
 * Zobrazi uvodne menu aplikacie (neprihlaseny uzivatel)
 * @param sockfd
 * @param buffer
 * @param noExit - adresa kam sa ulozi priznak ci uzivatel ukoncil aplikaciu (0 - koniec, 1 - pokracuje sa)
 * @param username - adresa kam sa ulozi meno prihlaseneho uzivatela
 * @return - cislo moznosti ktoru si uzivatel zvolil
 */
int showStartMenu(int* sockfd, char* buffer, int* noExit, char* username) {
    printf("\n### Chat app ###\n\n");
    printf("Menu:\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n\n");
    
    int option;
    scanf("%d", &option);
    
    char username_input[USER_USERNAME_MAX_LENGTH];
    char password_input[USER_PASSWORD_MAX_LENGTH];
    int value;
    
    switch(option) {
        case 1:
            // Cyklus 3 pokusy na zadanie mena a hesla
            *noExit = 0;
            printf("Enter your login:\t");
            scanf("%s", username_input);
            printf("\nEnter your password:\t");
            scanf("%s", password_input);

            if (strlen(username_input) > 0 && strlen(password_input) > 0) {

                ACCOUNT_CREDENTIALS c = {
                    username_input,
                    password_input
                };

                value = loginClient(sockfd, buffer, &c);
                if (value == SOCK_RES_OK) {

                    // Uspesne prihlasenie
                    printf("Login successful.\n");
                    memcpy(username, c.username, strlen(c.username));
                    *noExit = 1;
                    break;

                } else {
                    printf("Login failed.\n");
                    break;
                }

            } else {
                printf("Login failed.\n");
                break;
            }
            break;
            
        case 2:
            printf("Enter your login:\t");
            scanf("%s", username_input);
            printf("\nEnter your password:\t");
            scanf("%s", password_input);
            
            ACCOUNT_CREDENTIALS c = {username_input, password_input};
            value = registerNewClient(sockfd, buffer, &c);
            if (value == SOCK_RES_OK) {
                printf("\nRegistration successful!\n");
                sleep(1);
                *noExit = 1;
                break;
            } 
            else {
                // Chyba pri registracii
                printf("\nRegistration failed! Entered username is not available.\n");
                sleep(1);
                *noExit = 1;
                break;
            }
            
        default:
            printf("Bye!\n");
            *noExit = 0;
            break;
    }
    
    return option;
}


/**
 * Zobrazi menu pre prihlaseneho uzivatela
 * @param sockfd
 * @param buffer
 * @param noExit - adresa kam sa ulozi priznak ci uzivatel ukoncil aplikaciu (0 - koniec, 1 - pokracuje sa)
 * @param username - meno prihlaseneho uzivatela
 */
void showMenuAuthenticated(int* sockfd, char* buffer, int* noExit, char* username) {
    printf("\n\n\n### Chat app ###\n\n");
    printf("You are logged as %s\n", username);
    printf("Menu:\n");
    printf("1. Start chat\n");
    printf("2. My contacts\n");
    printf("3. Exit\n\n");
    
    int option;
    scanf("%d", &option);
    
    char* value1 = malloc(USER_USERNAME_MAX_LENGTH * sizeof(char));
    char* value2 = malloc(30 * sizeof(char));
    
    switch(option) {
        case 1:            
            printf("NOT IMPLEMENTED YET!\n");
            *noExit = 1;
            break;
            
        case 2:
            printf("\n\n\n### Chat app ###\n\n");
            printf("You are logged as %s\n", username);
            
            showUserContacts(sockfd, buffer);       // Zobrazi kontakty uzivatela
            
            printf("Menu:\n");
            printf("1. Add contact\n");
            printf("2. Delete contact\n");
            printf("3. Cancel\n\n");
            
            scanf("%d", &option);
            
            switch(option) {
                case 1:            
                    printf("Enter username of user you want to add to your contacts:\n");
                    scanf("%s", value1);
                    
                    addNewContact(sockfd, buffer, value1);
                    
                    printf("NOT IMPLEMENTED YET!\n");
                    *noExit = 1;
                    break;
            
                case 2:            
                    printf("Enter username of user you want to remove from your contacts:\n");
                    scanf("%s", value1);
                    
                    deleteContact(sockfd, buffer, value1);
                    
                    printf("NOT IMPLEMENTED YET!\n");
                    *noExit = 1;
                    break;
            
                default:            
                    *noExit = 1;
                    break;
            }
    
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