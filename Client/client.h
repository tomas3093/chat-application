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
int registerNewClient(int* sockfd, char* buffer, char* username, char* password)
{
    if (strlen(username) <= 0 || strlen(password) <= 0) {
        return newErrorMessage(buffer, "Bad data format");
    }
    
    addMessageCode(buffer, SOCK_REQ_REGISTER);
    
    strcat(buffer, username);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, password);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
    }
    
    // Zistenie s akym stavom prebehla registracia
    int responseCode = getMessageCode(buffer);
    
    // Vypis odpovede
    char* response = getSecondBufferArgument(buffer);
    puts(response);
    
    return responseCode;
}


/* Poziadavka na prihlasenie uzivatela do jeho konta
 * sockfd - inicializovany a pripojeny socket
 * buffer - premenna v ktorej sa vrati odpoved servera
 * credentials - prihlasovacie udaje
 * return - stav s akym funkcia skoncila
*/
int loginClient(int* sockfd, char* buffer, char* username, char* password)
{
    if (strlen(username) <= 0 || strlen(password) <= 0) {
        return newErrorMessage(buffer, "Bad data format");
    }
    
    addMessageCode(buffer, SOCK_REQ_LOGIN);
    
    strcat(buffer, username);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, password);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
    }
    
    // Zistenie s akym stavom prebehlo prihlasenie
    int responseCode = getMessageCode(buffer);
    
    // Vypis odpovede
    char* response = getSecondBufferArgument(buffer);
    puts(response);
    
    return responseCode;
}


/**
 * Poziada o kontakty aktualne prihlaseneho uzivatela a vypise ich na konzolu
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @return - stav s akym funkcia skoncila
 */
int showUserContacts(int* sockfd, char* buffer) {
    addMessageCode(buffer, SOCK_REQ_GET_CONTACTS);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
    }
    
    // Zistenie s akym stavom prebehla poziadavka
    int responseCode = getMessageCode(buffer);
    if (responseCode == SOCK_RES_OK) {
        
        // Naparsovanie buffra a vypisanie vsetkych argumentov (kontaktov)
        char* username = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
        memset(username, 0, USER_USERNAME_MAX_LENGTH);

        const int SKIP_COUNT = 1; // kolko argumentov sa ma preskocit
        int count = 0;  // pocitadlo argumentov
        int j = 0;
        for (int i = 0; i <= strlen(buffer); i++) {
            if (buffer[i] == '\0') {
                if (count >= SKIP_COUNT && strlen(username) > 0) {
                    printf("%d. %s\n", count - SKIP_COUNT + 1, username);
                }
                break;
                
            } else if (buffer[i] != SOCK_SPECIAL_SYMBOL && strlen(username) < USER_USERNAME_MAX_LENGTH) {
                username[j] = buffer[i];
                j++;
            } else {
                if (count >= SKIP_COUNT && strlen(username) > 0) {
                    printf("%d. %s\n", count - SKIP_COUNT + 1, username);
                }

                memset(username, 0, USER_USERNAME_MAX_LENGTH);
                j = 0;
                count++;
            }
        }
        
        if (count <= SKIP_COUNT && strlen(username) == 0) {
            printf("You have 0 contacts.\n");
        }
        
        free(username);
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
       return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
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
       return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
    }
    
    // Zistenie s akym stavom prebehlo odobratie z kontaktov
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


/**
 * Zisti ci je mozne zacat chat s danym uzivatelom (ci taky existuje a maju sa navzajom v kontaktoch)
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @param contactUsername - meno uzivatela, s ktorym chceme zacat chat
 * @return - stav s akym funkcia skoncila 
 */
int startChat(int* sockfd, char* buffer, char* contactUsername) {
    addMessageCode(buffer, SOCK_REQ_START_CHAT);
    
    strcat(buffer, contactUsername);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
    }
    
    // Zistenie s akym stavom prebehlo
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


/**
 * Vypise niekolko poslednych sprav prijatych od daneho uzivatela
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @param contactUsername - meno uzivatela, ktoreho spravy chceme zobrazit
 * @return - stav s akym funkcia skoncila
 */
int showRecentMessages(int* sockfd, char* buffer, char* contactUsername) {
    addMessageCode(buffer, SOCK_REQ_GET_RECENT_MESSAGES);
    strcat(buffer, contactUsername);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
       return newErrorMessage(buffer, "Error writing to socket");
    }

    
    // Nacitanie sprav zo servera
    int responseCode = SOCK_RES_OK;
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n#####################\nCHAT with %s\n#####################\n", contactUsername);
    while (responseCode == SOCK_RES_OK) {    
        memset(buffer, 0, SOCK_BUFFER_LENGTH);
        n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
        if (n < 0)
        {
            return newErrorMessage(buffer, "Error reading from socket");
        }
        if (n == 0) {
            return newErrorMessage(buffer, "Lost connection to server");
        }


        // Zistenie s akym stavom prebehla poziadavka
        responseCode = getMessageCode(buffer);
        if (responseCode == SOCK_RES_OK) {

            // Vypisanie spravy
            char* sender = getSecondBufferArgument(buffer);     // Odosielatel spravy
            char* messageText = getThirdBufferArgument(buffer); // Text spravy
            char* abbr = strcmp(sender, contactUsername) == 0 ? contactUsername : "me";
            printf("%s: %s", abbr, messageText);
            
            free(sender);
            free(messageText);
            
            // Odpoved ze sprava bola dorucena
            addMessageCode(buffer, SOCK_RES_OK);
            int n = write(*sockfd, buffer, strlen(buffer));
            if (n < 0)
            {
               return newErrorMessage(buffer, "Error writing to socket");
            }
        }
    }

    return SOCK_RES_OK;
}


/**
 * Posle spravu uzivatelovi
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @param contactUsername - meno uzivatela, ktoremu chceme odoslat spravu
 * @param message - text spravy
 * @return - stav s akym funkcia skoncila
 */
int sendMessage(int* sockfd, char* buffer, char* contactUsername, char* message) {
    
    // Chybne udaje
    if (strlen(contactUsername) <= 0 || strlen(message) <= 0) {
        return newErrorMessage(buffer, "Bad data format");
    }
    
    addMessageCode(buffer, SOCK_REQ_SEND_MESSAGE);
    strcat(buffer, contactUsername);
    strcat(buffer, &SOCK_SPECIAL_SYMBOL);
    strcat(buffer, message);

    // Pošleme správu cez socket servru.
    int n = write(*sockfd, buffer, strlen(buffer) + 1);
    if (n < 0)
    {
       return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
    }
    
    // Zistenie s akym stavom prebehla operacia
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


/**
 * Odstrani konto zadaneho uzivatela
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @param userToDelete - meno uzivatela, ktory sa ma vymazat
 * @return - stav s akym funkcia skoncila
 */
int deleteAccount(int* sockfd, char* buffer) {
    
    // Odpoved
    addMessageCode(buffer, SOCK_REQ_DELETE_ACCOUNT);
    
    int n = write(*sockfd, buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error writing to socket");
    }

    // Načítame odpoveď od servra do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    n = read(*sockfd, buffer, SOCK_BUFFER_LENGTH - 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error reading from socket");
    }
    
    // Zistenie s akym stavom prebehla operacia
    int responseCode = getMessageCode(buffer);
    
    return responseCode;
}


/**
 * Zobrazi uvodne menu aplikacie (neprihlaseny uzivatel)
 * @param sockfd - inicializovany a pripojeny socket
 * @param buffer - premenna v ktorej sa vrati odpoved servera
 * @param username - adresa kam sa ulozi meno prihlaseneho uzivatela
 * @return - status klienta (znamena co sa ma robit dalej)
 */
int showStartMenu(int* sockfd, char* buffer, char* username) {
    printf("\n### Chat app ###\n\n");
    printf("Menu:\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n\n");
    
    int option;
    scanf("%d", &option);
    
    char username_input[USER_USERNAME_MAX_LENGTH];
    char password_input[USER_PASSWORD_MAX_LENGTH];
    int status;
    
    switch(option) {
        case 1:
            printf("Enter your login:\t");
            scanf("%s", username_input);
            printf("\nEnter your password:\t");
            scanf("%s", password_input);

            status = loginClient(sockfd, buffer, username_input, password_input);
            if (status == SOCK_RES_OK) {

                // Uspesne prihlasenie
                memcpy(username, username_input, strlen(username_input));
                return CLIENT_STATUS_AUTHENTICATED;
            }
            break;
            
        case 2:
            printf("Enter your login:\t");
            scanf("%s", username_input);
            printf("\nEnter your password:\t");
            scanf("%s", password_input);
            
            status = registerNewClient(sockfd, buffer, username_input, password_input);
            if (status == SOCK_RES_OK) {
                
                // Uspesna registracia
                sleep(1);
                break;
            }
            break;
            
        default:
            printf("Bye!\n");
            return CLIENT_STATUS_EXIT;
    }
    
    return CLIENT_STATUS_UNAUTHENTICATED;
}


/**
 * Zobrazi menu pre prihlaseneho uzivatela
 * @param sockfd
 * @param buffer
 * @param username - meno prihlaseneho uzivatela
 * @return - status klienta (znamena co sa ma robit dalej)
 */
int showMenuAuthenticated(int* sockfd, char* buffer, char* username) {
    printf("\n\n### Chat app ###\n\n");
    printf("You are logged as %s\n", username);
    printf("Menu:\n");
    printf("1. Start chat\n");
    printf("2. My contacts\n");
    printf("3. Delete account\n");
    printf("4. Logout\n");
    printf("5. Exit\n\n");
    
    int option;
    int status;
    scanf("%d", &option);
    
    char* value1 = malloc(USER_USERNAME_MAX_LENGTH * sizeof(char));
    char* value2 = malloc(CLIENT_MESSAGE_LENGTH * sizeof(char));
    
    switch(option) {
        case 1:            
            // Tu sa vypyta meno kontaktu a text spravy. Odosle sa na server a ak bol req uspesny
            // tak sa spusti nekonecny cyklus s chatom s danym uzivatelom
            
            printf("Enter username of user you want to chat with:\n");
            scanf("%s", value1);
            status = startChat(sockfd, buffer, value1);
            if (status == SOCK_RES_OK) {
                
                // Spustenie chatu
                printf("Chat with %s\nFor exit type 'exit'\n\n", value1);
                
                while (1) {
                    // Vypis predoslych sprav
                    status = showRecentMessages(sockfd, buffer, value1);
                    if (status != SOCK_RES_OK) {
                        printErrorMessage(buffer);
                        break;
                    }

                    printf("You: ");
                    fgets(value2, CLIENT_MESSAGE_LENGTH, stdin);
                    if (strcmp(value2, "exit\n") == 0) {
                        break;
                    }

                    if (strlen(value2) > 0 && strcmp(value2, "\n") != 0) {
                        status = sendMessage(sockfd, buffer, value1, value2);
                        if (status != SOCK_RES_OK) {
                            printErrorMessage(buffer);
                            break;
                        }
                    }
                }
            }
            break;
            
        case 2:
            printf("\n\n### Chat app ###\n\n");
            printf("You are logged as %s\n\n", username);
            
            printf("Your contacts:\n");
            showUserContacts(sockfd, buffer);       // Zobrazi kontakty uzivatela
            
            printf("\nMenu:\n");
            printf("1. Add contact\n");
            printf("2. Delete contact\n");
            printf("3. Cancel\n\n");
            
            scanf("%d", &option);
            
            switch(option) {
                case 1:            
                    printf("Enter username of user you want to add to your contacts:\n");
                    scanf("%s", value1);
                    
                    status = addNewContact(sockfd, buffer, value1);
                    if (status == SOCK_RES_OK) {
                        printf("\n%s has been added to your contacts\n", value1);
                    } else {
                        printf("\nAdding failed!\n");
                    }
                    break;
            
                case 2:            
                    printf("Enter username of user you want to remove from your contacts:\n");
                    scanf("%s", value1);
                    
                    status = deleteContact(sockfd, buffer, value1);
                    if (status == SOCK_RES_OK) {
                        printf("\n%s has been removed from your contacts\n", value1);
                    } else {
                        printf("\nRemoving failed!\n");
                    }
                    break;
            
                default:          
                    break;
            }
            break;
            
        case 3:
            printf("Do you really want to delete your account? (yes/N)\n");
            scanf("%s", value1);
            
            // Odstranenie konta uzivatela
            if (strcmp(value1, "yes") == 0) {
                status = deleteAccount(sockfd, buffer);
                if (status == SOCK_RES_OK) {
                    printf("Your account has been deleted!\n");
                    memset(username, 0, USER_USERNAME_MAX_LENGTH);  // Odhlasenie uzivatela
                    free(value1);
                    free(value2);
                    return CLIENT_STATUS_UNAUTHENTICATED;
                } else {
                    printf("Deletion failed!\n");
                }
            }
            break;
            
        case 4:
            printf("You have been logged out!\n");
            memset(username, 0, USER_USERNAME_MAX_LENGTH);  // Odhlasenie uzivatela
            free(value1);
            free(value2);
            return CLIENT_STATUS_UNAUTHENTICATED;
            
        default:
            free(value1);
            free(value2);
            printf("Bye!\n");
            return CLIENT_STATUS_EXIT;
    }
    
    free(value1);
    free(value2);
    return CLIENT_STATUS_AUTHENTICATED;
}