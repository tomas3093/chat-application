#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>

#include "../konstanty.h"
#include "../routines.h"


/**
 * Obsluha poziadavky pre registraciu noveho klienta
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param credentials - udaje noveho uzivatela
 */
void clientRegisterHandler(CLIENT_SOCKET* p, char* buffer, ACCOUNT_CREDENTIALS* credentials) {
    int registration_successful;    // priznak ci prebehla registracia uspesne
    char* messageText;              // odpoved
    
    // Validita zadanych udajov
    if (    strlen(credentials->username) >= USER_USERNAME_MIN_LENGTH && 
            strlen(credentials->username) <= USER_USERNAME_MAX_LENGTH &&
            strlen(credentials->password) >= USER_PASSWORD_MIN_LENGTH &&
            strlen(credentials->password) <= USER_PASSWORD_MAX_LENGTH) {

        // Kontrola ci je zadane meno unikatne
        int isUnique = 1;
        pthread_mutex_lock(p->accounts_mutex);
        for(int i = 0; i < *p->accounts_count; i++) {
            if (strcmp(p->accounts[i]->credentials->username, credentials->username) == 0 && *p->accounts[i]->active == 1) {
                isUnique = 0;
                break;
            }
        }
        
        // Je unikatne a je volne miesto
        if (isUnique == 1 && *p->accounts_count < CLIENT_MAX_ACCOUNT_COUNT) {
            
            // Vytvorenie konta uzivatela
            USER_ACCOUNT* account = malloc(sizeof(USER_ACCOUNT));
            account->contacts = malloc(sizeof(int*) * CLIENT_MAX_ACCOUNT_COUNT);
            account->active = malloc(sizeof(int));
            account->credentials = credentials;
            *account->active = 1;
            
            // Inicializacia pola kontaktov
            for(int i = 0; i < CLIENT_MAX_ACCOUNT_COUNT; i++) {
                account->contacts[i] = malloc(sizeof(int));
                *account->contacts[i] = 0;
            }
        
            p->accounts[*p->accounts_count] = account;
            (*p->accounts_count)++;
            
            registration_successful = 1;
            messageText = "Registracia bola uspesna.";
        } else {
            free(credentials);
            registration_successful = 0;
            messageText = "Registracia zlyhala. Nie je dostatocna velkost pamate.";
        }
        pthread_mutex_unlock(p->accounts_mutex);
        
    } else {
        free(credentials);
        registration_successful = 0;
        messageText = "Registracia zlyhala. Meno alebo heslo nema spravnu dlzku.";
    }

    // Odpoved klientovi
    const int messageCode = registration_successful == 1 ? 
        SOCK_RES_OK : SOCK_RES_FAIL;
    
    addMessageCode(buffer, messageCode);
    strcat(buffer, messageText);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        newErrorMessage(buffer, "Error writing to socket");
    }
}


/**
 * Obsluha poziadavky pre prihlasenie klienta
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param credentials - udaje uzivatela, ktory sa chce prihlasit
 */
void clientLoginHandler(CLIENT_SOCKET* p, char* buffer, ACCOUNT_CREDENTIALS* credentials) {
    int login_successful;   // priznak ci prebehlo prihlasenie uspesne
    char* messageText;      // odpoved
    
    // Validita zadanych udajov
    if (    strlen(credentials->username) >= USER_USERNAME_MIN_LENGTH && 
            strlen(credentials->username) <= USER_USERNAME_MAX_LENGTH &&
            strlen(credentials->password) >= USER_PASSWORD_MIN_LENGTH &&
            strlen(credentials->password) <= USER_PASSWORD_MAX_LENGTH) {
        
        // Kontrola ci je zadane meno registrovane, je spravne heslo a aktivny ucet
        int isValid = 0;
        pthread_mutex_lock(p->accounts_mutex);
        for(int i = 0; i < *p->accounts_count; i++) {
            if (strcmp(p->accounts[i]->credentials->username, credentials->username) == 0 && *p->accounts[i]->active == 1) {
                if (strcmp(p->accounts[i]->credentials->password, credentials->password) == 0) {
                    isValid = 1;
                    break;
                } else {
                    // Nespravne heslo
                    break;
                }
            }
        }
        
        // Zadane udaje su spravne
        if (isValid == 1) {
            
            // Prihlasenie
            login_successful = 1;
            messageText = "Prihlasenie bolo uspesne.";
        } else {
            login_successful = 0;
            messageText = "Prihlasenie nebolo uspesne. Nespravne meno alebo heslo.";
        }
        pthread_mutex_unlock(p->accounts_mutex);
        
    } else {
        login_successful = 0;
        messageText = "Prihlasenie nebolo uspesne.";
    }

    // Odpoved klientovi    
    const int messageCode = login_successful == 1 ? 
        SOCK_RES_OK : SOCK_RES_FAIL;
    
    addMessageCode(buffer, messageCode);
    strcat(buffer, messageText);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        newErrorMessage(buffer, "Error writing to socket");
    }
}


/**
 * Obsluha poziadavky pre ziskanie vsetkych kontaktov daneho uzivatela
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param clientUsername - meno uzivatela
 */
void clientGetContactsHandler(CLIENT_SOCKET* p, char* buffer, char* clientUsername) {
    char* messageText = malloc(sizeof(char) * SOCK_MESSAGE_LENGTH);     // odpoved
    
    int** contacts;
    
    // Kontrola ci je zadane meno registrovane a aktivny ucet
    int isValid = 0;
    int i;
    pthread_mutex_lock(p->accounts_mutex);
    for(i = 0; i < *p->accounts_count; i++) {
        if (strcmp(p->accounts[i]->credentials->username, clientUsername) == 0 && *p->accounts[i]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        contacts = p->accounts[i]->contacts;        
    } 
    
    
    // Odpoved klientovi    
    int messageCode;
    
    if (isValid == 1) {
        // Vytvorenie spravy s jednotlivymi kontaktmi
        for (int j = 0; j < *p->accounts_count; j++) {
            if (i != j && *contacts[j] == 1) {
                strcat(messageText, p->accounts[j]->credentials->username);
                strcat(messageText, &SOCK_SPECIAL_SYMBOL);
            }
        }
        
        if (strlen(messageText) > 0) {
            messageText[strlen(messageText) - 1] = '\0';
        }
        
        messageCode = SOCK_RES_OK;
    } else {
        // Neuspech
        messageCode = SOCK_RES_FAIL;
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    addMessageCode(buffer, messageCode);
    strcat(buffer, messageText);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        newErrorMessage(buffer, "Error writing to socket");
    }
    
    free(messageText);
}


/**
 * Funkcia prida prvemu uzivatelovi medzi kontakty druheho uzivatela a naopak
 * @param p - data
 * @param currentUser - prvy uzivatel
 * @param userToAdd - druhy uzivatel
 * @return - stav s akym vykonavanie skoncilo
 */
int addNewContact(CLIENT_SOCKET* p, char* currentUser, char* userToAdd) {
    int** currentUserContacts;     // kontakty aktualneho usera
    int** userToAddContacts;       // kontakty pridavaneho usera
    
    // ZISKANIE KONTAKTOV OBIDVOCH USEROV
    // Aktualny uzivatel
    // Kontrola ci je meno aktualneho uzivatela registrovane a ma aktivny ucet
    int isValid = 0;
    int currentUserIndex;  // index konta aktualneho uzivatela
    pthread_mutex_lock(p->accounts_mutex);
    for(currentUserIndex = 0; currentUserIndex < *p->accounts_count; currentUserIndex++) {
        if (strcmp(p->accounts[currentUserIndex]->credentials->username, currentUser) == 0 && *p->accounts[currentUserIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        currentUserContacts = p->accounts[currentUserIndex]->contacts;        
    } else {
        pthread_mutex_unlock(p->accounts_mutex);
        return SOCK_RES_FAIL;
    }
    
    
    // Pridavany uzivatel 
    // Kontrola ci je meno pridavaneho uzivatela registrovane a ma aktivny ucet
    isValid = 0;
    int userToAddIndex;  // index konta pridavaneho uzivatela
    for(userToAddIndex = 0; userToAddIndex < *p->accounts_count; userToAddIndex++) {
        if (strcmp(p->accounts[userToAddIndex]->credentials->username, userToAdd) == 0 && *p->accounts[userToAddIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        userToAddContacts = p->accounts[userToAddIndex]->contacts;        
    } else {
        pthread_mutex_unlock(p->accounts_mutex);
        return SOCK_RES_FAIL;
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    
    // Zistenie ci tento kontakt uzivatelia este nemaju
    if (*currentUserContacts[userToAddIndex] == 1 || *userToAddContacts[currentUserIndex] == 1) {
        return SOCK_RES_FAIL;
    }

    // Pridanie medzi kontakty
    *currentUserContacts[userToAddIndex] = 1;
    *userToAddContacts[currentUserIndex] = 1;

    return SOCK_RES_OK;
}


/**
 * Obsluha poziadavky pre pridanie noveho kontaktu
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param currentUser - meno aktualne prihlaseneho uzivatela
 * @param userToAdd - meno uzivatela, ktoreho chceme pridat medzi kontakty
 */
void clientAddNewContactHandler(CLIENT_SOCKET* p, char* buffer, char* currentUser, char* userToAdd) {
    int messageCode = addNewContact(p, currentUser, userToAdd);
    
    sendResponseStatus(p, buffer, messageCode);
}


/**
 * Funkcia odstrani prvemu uzivatelovi z kontaktov druheho uzivatela a naopak
 * @param p - data
 * @param currentUser - prvy uzivatel
 * @param userToDelete - druhy uzivatel
 * @return - stav s akym vykonavanie skoncilo
 */
int deleteContact(CLIENT_SOCKET* p, char* currentUser, char* userToDelete) {
    int** currentUserContacts;     // kontakty aktualneho usera
    int** userToDeleteContacts;    // kontakty odoberaneho usera
    
    // ZISKANIE KONTAKTOV OBIDVOCH USEROV
    // Aktualny uzivatel
    // Kontrola ci je meno aktualneho uzivatela registrovane a ma aktivny ucet
    int isValid = 0;
    int currentUserIndex;  // index konta aktualneho uzivatela v poli accounts
    pthread_mutex_lock(p->accounts_mutex);
    for(currentUserIndex = 0; currentUserIndex < *p->accounts_count; currentUserIndex++) {
        if (strcmp(p->accounts[currentUserIndex]->credentials->username, currentUser) == 0 && *p->accounts[currentUserIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        currentUserContacts = p->accounts[currentUserIndex]->contacts;        
    } else {
        pthread_mutex_unlock(p->accounts_mutex);
        return SOCK_RES_FAIL;
    }
    
    
    // Odoberany uzivatel 
    // Kontrola ci je meno odoberaneho uzivatela registrovane
    isValid = 0;
    int userToDeleteIndex;  // index konta odoberaneho uzivatela v poli accounts
    for(userToDeleteIndex = 0; userToDeleteIndex < *p->accounts_count; userToDeleteIndex++) {
        if (strcmp(p->accounts[userToDeleteIndex]->credentials->username, userToDelete) == 0) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        userToDeleteContacts = p->accounts[userToDeleteIndex]->contacts;        
    } else {
        pthread_mutex_unlock(p->accounts_mutex);
        return SOCK_RES_FAIL;
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    
    // Odobratie kontaktov
    *currentUserContacts[userToDeleteIndex] = 0;
    *userToDeleteContacts[currentUserIndex] = 0;
    
    return SOCK_RES_OK;
}


/**
 * Obsluha poziadavky pre odstranenie kontaktu
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param currentUser - meno aktualne prihlaseneho uzivatela
 * @param userToDelete - meno uzivatela, ktoreho chceme odstranit z kontaktov
 */
void clientDeleteContactHandler(CLIENT_SOCKET* p, char* buffer, char* currentUser, char* userToDelete) {
    int messageCode = deleteContact(p, currentUser, userToDelete);
    
    sendResponseStatus(p, buffer, messageCode);
}


/**
 * Funkcia skontroluje ci zadany uzivatelia existuju a ci su navzajom v kontakte
 * @param p
 * @param currentUser
 * @param userToChat
 * @return - stav s akym vykonavanie skoncilo
 */
int startChat(CLIENT_SOCKET* p, char* buffer, char* currentUser, char* userToChat) {
    int** currentUserContacts;
    int** userToChatContacts;
    
    // Aktualny uzivatel
    // Kontrola ci je meno aktualneho uzivatela registrovane a ma aktivny ucet
    int isValid = 0;
    int currentUserIndex;  // index konta aktualneho uzivatela
    pthread_mutex_lock(p->accounts_mutex);
    for(currentUserIndex = 0; currentUserIndex < *p->accounts_count; currentUserIndex++) {
        if (strcmp(p->accounts[currentUserIndex]->credentials->username, currentUser) == 0 && *p->accounts[currentUserIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        currentUserContacts = p->accounts[currentUserIndex]->contacts;        
    } else {
        pthread_mutex_unlock(p->accounts_mutex);
        return newErrorMessage(buffer, "No such user!");
    }
    
    
    // Pridavany uzivatel 
    // Kontrola ci je meno uzivatela s ktorym sa ma zacat chat registrovane a ma aktivny ucet
    isValid = 0;
    int userToChatIndex;  // index konta uzivatela, s ktorym sa ma zacat chat
    for(userToChatIndex = 0; userToChatIndex < *p->accounts_count; userToChatIndex++) {
        if (strcmp(p->accounts[userToChatIndex]->credentials->username, userToChat) == 0 && *p->accounts[userToChatIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        userToChatContacts = p->accounts[userToChatIndex]->contacts;
    } else {
        pthread_mutex_unlock(p->accounts_mutex);
        return newErrorMessage(buffer, "No such user!");
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    
    // Zistenie ci su uzivatelia navzajom v kontakte
    if (*currentUserContacts[userToChatIndex] == 0 || *userToChatContacts[currentUserIndex] == 0) {
        return newErrorMessage(buffer, "You don't have this user in your contacts!");
    }
        
    return SOCK_RES_OK;
}


/**
 * Obsluha poziadavky pre zistenie ci je mozne zacat chat so zadanym uzivatelom
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param currentUser - meno aktualne prihlaseneho uzivatela
 * @param userToChat - meno uzivatela s ktorym chceme zacat chat
 */
void clientStartChatHandler(CLIENT_SOCKET* p, char* buffer, char* currentUser, char* userToChat) {
    startChat(p, buffer, currentUser, userToChat);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        newErrorMessage(buffer, "Error writing to socket");
    }
}


/**
 * Odosle spravu inemu uzivatelovi
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param sender - meno aktualne prihlaseneho uzivatela
 * @param recipient - meno uzivatela, pre ktoreho je sprava urcena
 * @param messageText - text samotnej spravy
 * @return - stav s akym vykonavanie skoncilo
 */
int clientSendMessage(CLIENT_SOCKET* p, char* buffer, char* sender, char* recipient, char* messageText) {
    
    // Ak je prazdna sprava
    if (strlen(messageText) <= 0) {
        return SOCK_RES_FAIL;
    }

    // Vytvorenie spravy a jej ulozenie do pola
    MESSAGE* message = malloc(sizeof(MESSAGE));
    message->unread = malloc(sizeof(int));
    message->sender = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
    message->recipient = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
    message->text = malloc(sizeof(char) * CLIENT_MESSAGE_LENGTH);
    *message->unread = 1;
    memcpy(message->sender, sender, strlen(sender));
    memcpy(message->recipient, recipient, strlen(recipient));
    memcpy(message->text, messageText, strlen(messageText));
    
    pthread_mutex_lock(p->messages_mutex);
    if (*p->messages_count >= SERVER_MAX_MESSAGES_COUNT) {
        pthread_mutex_unlock(p->messages_mutex);
        return newErrorMessage(buffer, "Error: Server memory is full.");
    }
    
    p->messages[*p->messages_count] = message;
    (*p->messages_count)++;
    pthread_mutex_unlock(p->messages_mutex);
    
    return SOCK_RES_OK;
}


/**
 * Obsluha poziadavky pre odoslanie spravy inemu uzivatelovi
 * @param p
 * @param buffer
 * @param sender
 * @param recipient
 * @param messageText
 */
void clientSendMessageHandler(CLIENT_SOCKET* p, char* buffer, char* sender, char* recipient, char* messageText) {
    int messageCode = clientSendMessage(p, buffer, sender, recipient, messageText);
    
    sendResponseStatus(p, buffer, messageCode);
}


/**
 * Odosle klientovi najnovsie spravy z chatu s konkretnym uzivatelom
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param currentUser - meno aktualne prihlaseneho uzivatela
 * @param sender - meno uzivatela, od ktoreho chceme zobrazit nedavne spravy
 * @return - stav s akym vykonavanie skoncilo
 */
int clientGetRecentMessages(CLIENT_SOCKET* p, char* buffer, char* currentUser, char* sender) {
    
    // Zistenie ci su vzajomne kontakty
    int** currentUserContacts;     // kontakty aktualneho uzivatela
    
    // Ziskanie kontaktov
    int isValid = 0;
    int currentUserIndex;  // index konta aktualneho uzivatela v poli accounts
    pthread_mutex_lock(p->accounts_mutex);
    for(currentUserIndex = 0; currentUserIndex < *p->accounts_count; currentUserIndex++) {
        if (strcmp(p->accounts[currentUserIndex]->credentials->username, currentUser) == 0 && *p->accounts[currentUserIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        currentUserContacts = p->accounts[currentUserIndex]->contacts; 
        
        // Najdenie ci su vzajomne kontakty
        isValid = 0;
        for(int i = 0; i < *p->accounts_count; i++) {
            if (*currentUserContacts[i] == 1 && strcmp(p->accounts[i]->credentials->username, sender)) {
                isValid = 1;
                break;
            }
        }
        if (isValid == 0) {
            pthread_mutex_unlock(p->accounts_mutex);
            return SOCK_RES_FAIL;
        }

        
    } else {
        pthread_mutex_unlock(p->accounts_mutex);
        return SOCK_RES_FAIL;
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    
    
    MESSAGE* messages[CLIENT_MAX_RECEIVED_MESSAGES_COUNT];     // spravy pre uzivatela
    
    // Ziskanie relevantnych sprav
    int count = 0;  // Pocet najdenych sprav
    pthread_mutex_lock(p->messages_mutex);
    for (int i = 0; i < *p->messages_count; i++) {
        if (count >= CLIENT_MAX_RECEIVED_MESSAGES_COUNT) {
            break;
        }

        if ((strcmp(p->messages[i]->recipient, currentUser) == 0 && strcmp(p->messages[i]->sender, sender) == 0) ||
                (strcmp(p->messages[i]->recipient, sender) == 0 && strcmp(p->messages[i]->sender, currentUser) == 0)) {
            *p->messages[i]->unread = 0;
            messages[count] = p->messages[i];
            count++;
        }
    }
    pthread_mutex_unlock(p->messages_mutex);
    
    
    // Odoslanie sprav cez socket
    for (int i = 0; i < count; i++) {
        addMessageCode(buffer, SOCK_RES_OK);
        strcat(buffer, messages[i]->sender);
        strcat(buffer, &SOCK_SPECIAL_SYMBOL);
        strcat(buffer, messages[i]->text);
        
        int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
        if (n < 0)
        {
            return newErrorMessage(buffer, "Error writing to socket");
        }
    }
    
    // Ked uz nie su ziadne spravy, tak sa posle kod FAIL
    addMessageCode(buffer, SOCK_RES_FAIL);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        return newErrorMessage(buffer, "Error writing to socket");
    }
    
    return SOCK_RES_OK;
}


/**
 * Obsluha poziadavky pre zobrazenie sprav od konkretneho uzivatela
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param currentUser - meno aktualne prihlaseneho uzivatela
 * @param sender - meno uzivatela, od ktoreho chceme zobrazit nedavne spravy
 */
void clientGetRecentMessagesHandler(CLIENT_SOCKET* p, char* buffer, char* currentUser, char* sender) {
    int messageCode = clientGetRecentMessages(p, buffer, currentUser, sender);
    
    sendResponseStatus(p, buffer, messageCode);
}


/**
 * Obsluha poziadavky pre vymazanie konta uzivatela
 * @param p - data
 * @param buffer - buffer pre spravu klientovi
 * @param userToDelete - meno uzivatela, ktoreho konto chceme vymazat
 */
void clientDeleteAccount(CLIENT_SOCKET* p, char* buffer, char* userToDelete) {
    
    int success = 0;
    int userIndex;  // index konta mazaneho uzivatela v poli accounts
    
    // Najdenie uzivatela
    pthread_mutex_lock(p->accounts_mutex);
    for(userIndex = 0; userIndex < *p->accounts_count; userIndex++) {
        if (strcmp(p->accounts[userIndex]->credentials->username, userToDelete) == 0 && *p->accounts[userIndex]->active == 1) {
            *p->accounts[userIndex]->active = 0;
            success = 1;
            break;
        }
    }
    
    // Odstranenie uzivatela zo vsetkych kontaktov
    for(int i = 0; i < *p->accounts_count; i++) {
        *p->accounts[i]->contacts[userIndex] = 0;
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    if (success == 1) {
        sendResponseStatus(p, buffer, SOCK_RES_OK);
        memset(userToDelete, 0, USER_USERNAME_MAX_LENGTH);
    } else {
        sendResponseStatus(p, buffer, SOCK_RES_FAIL);
    }
}


/**
 * Obsluha daneho klienta v jeho vlakne
 * @return 
 */
void* clientHandler(void* args) {
    
    CLIENT_SOCKET* p = (CLIENT_SOCKET*)args;
    
    // Meno aktualne prihlaseneho uzivatela
    char* username = malloc(sizeof(char) * USER_USERNAME_MAX_LENGTH);
    
    char buffer[SOCK_BUFFER_LENGTH];   // buffer pre spravy odosielane medzi serverom a klientom
    int read_size;  // Dlzka precitanej spravy z buffera
    
    // Hlavny cyklus s komunikaciou medzi serverom a klientom
    // Načítame správu od klienta cez socket do buffra.
    memset(buffer, 0, SOCK_BUFFER_LENGTH);
    while ((read_size = recv(*(p->client_sock), buffer, SOCK_BUFFER_LENGTH - 1, 0)) > 0) {

        // Zistenie typu spravy a zavolanie danej obsluhy
        int messageCode = getMessageCode(buffer);

        if (messageCode == SOCK_REQ_REGISTER) {
            ACCOUNT_CREDENTIALS* credentials = getCredentialsFromBuffer(buffer);
            clientRegisterHandler(p, buffer, credentials);
        } else if (messageCode == SOCK_REQ_LOGIN) {
            ACCOUNT_CREDENTIALS* credentials = getCredentialsFromBuffer(buffer);
            username = credentials->username;
            clientLoginHandler(p, buffer, credentials);
            free(credentials);
        } else if (messageCode == SOCK_REQ_DELETE_ACCOUNT) {
            clientDeleteAccount(p, buffer, username);
        } else if (messageCode == SOCK_REQ_ADD_CONTACT) {
            char* userToAdd = getSecondBufferArgument(buffer);
            clientAddNewContactHandler(p, buffer, username, userToAdd);
            free(userToAdd);
        } else if (messageCode == SOCK_REQ_DELETE_CONTACT) {
            char* userToDelete = getSecondBufferArgument(buffer);
            clientDeleteContactHandler(p, buffer, username, userToDelete);          // TODO
            free(userToDelete);
        } else if (messageCode == SOCK_REQ_GET_CONTACTS) {
            clientGetContactsHandler(p, buffer, username);
        } else if (messageCode == SOCK_REQ_START_CHAT) {
            char* userToChat = getSecondBufferArgument(buffer);
            clientStartChatHandler(p, buffer, username, userToChat);
            free(userToChat);
        } else if (messageCode == SOCK_REQ_SEND_MESSAGE) {
            char* recipient = getSecondBufferArgument(buffer);
            char* messageText = getThirdBufferArgument(buffer);
            clientSendMessageHandler(p, buffer, username, recipient, messageText);
            free(recipient);
            free(messageText);
        } else if (messageCode == SOCK_REQ_GET_RECENT_MESSAGES) {
            char* sender = getSecondBufferArgument(buffer);
            clientGetRecentMessagesHandler(p, buffer, username, sender);
            free(sender);
        } else if (messageCode == SOCK_REQ_LOGOUT) {
            // TODO
        } else {
            puts("Error: Unknown request");
        }
        
        // Vymazanie buffra
        memset(buffer, 0, SOCK_BUFFER_LENGTH);
    }

    if (read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if (read_size < 0) {
        perror("Receive failed");
    }         
    
    free(username);
    free(p->client_sock);
    free(p);
      
    return 0;
}