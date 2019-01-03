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

        // Vytvorenie konta uzivatela
        USER_ACCOUNT* account = malloc(sizeof(USER_ACCOUNT));
        account->credentials = credentials;
        account->active = 1;
                
        // Kontrola ci je zadane meno unikatne
        int isUnique = 1;
        pthread_mutex_lock(p->accounts_mutex);
        for(int i = 0; i < *p->accounts_count; i++) {
            if (strcmp(p->accounts[i]->credentials->username, credentials->username) == 0) {
                isUnique = 0;
                break;
            }
        }
        
        // Je unikatne a je volne miesto
        if (isUnique == 1 && *p->accounts_count < CLIENT_MAX_ACCOUNT_COUNT) {
            
            // Vytvorenie kontaktov uzivatela a registracia
            USER_CONTACTS* user_contacts = malloc(sizeof(USER_CONTACTS));
            user_contacts->contacts = malloc(sizeof(USER_ACCOUNT) * CLIENT_MAX_CONTACTS_COUNT);
            user_contacts->contacts_count = malloc(sizeof(int));
        
            p->accounts[*p->accounts_count] = account;
            p->contacts[*p->accounts_count] = user_contacts;
            (*p->accounts_count)++;
            
            registration_successful = 1;
            messageText = "Registracia bola uspesna.";
        } else {
            free(account->credentials);
            free(account);
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
        perror("Error writing to socket");
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
            if (strcmp(p->accounts[i]->credentials->username, credentials->username) == 0 && p->accounts[i]->active == 1) {
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
        perror("Error writing to socket");
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
    
    USER_CONTACTS* c;
    
    // Kontrola ci je zadane meno registrovane a aktivny ucet
    int isValid = 0;
    int i;
    pthread_mutex_lock(p->accounts_mutex);
    for(i = 0; i < *p->accounts_count; i++) {
        if (strcmp(p->accounts[i]->credentials->username, clientUsername) == 0 && p->accounts[i]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        c = p->contacts[i];        
    } 
    pthread_mutex_unlock(p->accounts_mutex);
    
    // Odpoved klientovi    
    int messageCode;
    
    if (isValid == 1) {
        // Vytvorenie spravy s jednotlivymi kontaktmi
        for (int j = 0; j < *c->contacts_count; j++) {
            strcat(messageText, (*c->contacts[j]).credentials->username);
            if (j + 1 < *c->contacts_count) {
                strcat(messageText, &SOCK_SPECIAL_SYMBOL);
            }
        }
        
        messageCode = SOCK_RES_OK;
    } else {
        // Neuspech
        messageCode = SOCK_RES_FAIL;
    }

    addMessageCode(buffer, messageCode);
    strcat(buffer, messageText);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
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
    USER_CONTACTS* currentUserContacts;     // kontakty aktualneho usera
    USER_CONTACTS* userToAddContacts;       // kontakty pridavaneho usera
    
    // Aktualny uzivatel
    // Kontrola ci je meno aktualneho uzivatela registrovane a ma aktivny ucet
    int isValid = 0;
    int currentUserIndex;  // index konta aktualneho uzivatela
    pthread_mutex_lock(p->accounts_mutex);
    for(currentUserIndex = 0; currentUserIndex < *p->accounts_count; currentUserIndex++) {
        if (strcmp(p->accounts[currentUserIndex]->credentials->username, currentUser) == 0 && p->accounts[currentUserIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        currentUserContacts = p->contacts[currentUserIndex];        
    } else {
        return SOCK_RES_FAIL;
    }
    
    
    // Pridavany uzivatel 
    // Kontrola ci je meno pridavaneho uzivatela registrovane a ma aktivny ucet
    isValid = 0;
    int userToAddIndex;  // index konta pridavaneho uzivatela
    for(userToAddIndex = 0; userToAddIndex < *p->accounts_count; userToAddIndex++) {
        if (strcmp(p->accounts[userToAddIndex]->credentials->username, userToAdd) == 0 && p->accounts[userToAddIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        userToAddContacts = p->contacts[userToAddIndex];        
    } else {
        return SOCK_RES_FAIL;
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    
    // Ci uzivatelia maju este volne miesto na dalsi kontakt
    if (*currentUserContacts->contacts_count < CLIENT_MAX_CONTACTS_COUNT && 
            *userToAddContacts->contacts_count < CLIENT_MAX_CONTACTS_COUNT) {
        
        // Zistenie ci tento kontakt uzivatelia este nemaju
        for (int i = 0; i < *currentUserContacts->contacts_count; i++) {
            if (strcmp((*currentUserContacts->contacts[i]).credentials->username, userToAdd) == 0) {
                return SOCK_RES_FAIL;
            }
        }
        
        for (int i = 0; i < *userToAddContacts->contacts_count; i++) {
            if (strcmp((*userToAddContacts->contacts[i]).credentials->username, currentUser) == 0) {
                return SOCK_RES_FAIL;
            }
        }
        
        // Pridanie medzi kontakty
        *currentUserContacts->contacts[*currentUserContacts->contacts_count] = *p->accounts[userToAddIndex];
        *userToAddContacts->contacts[*userToAddContacts->contacts_count] = *p->accounts[currentUserIndex];
        *currentUserContacts->contacts_count++;
        *userToAddContacts->contacts_count++;
        
        return SOCK_RES_OK;
    }
    
    return SOCK_RES_FAIL;
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
    addMessageCode(buffer, messageCode);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
    }
}


/**
 * Funkcia odstrani prvemu uzivatelovi z kontaktov druheho uzivatela a naopak
 * @param p - data
 * @param currentUser - prvy uzivatel
 * @param userToDelete - druhy uzivatel
 * @return - stav s akym vykonavanie skoncilo
 */
int deleteContact(CLIENT_SOCKET* p, char* currentUser, char* userToDelete) {
    USER_CONTACTS* currentUserContacts;     // kontakty aktualneho usera
    USER_CONTACTS* userToDeleteContacts;    // kontakty odoberaneho usera
    
    // Aktualny uzivatel
    // Kontrola ci je meno aktualneho uzivatela registrovane a ma aktivny ucet
    int isValid = 0;
    int currentUserIndex;  // index konta aktualneho uzivatela
    pthread_mutex_lock(p->accounts_mutex);
    for(currentUserIndex = 0; currentUserIndex < *p->accounts_count; currentUserIndex++) {
        if (strcmp(p->accounts[currentUserIndex]->credentials->username, currentUser) == 0 && p->accounts[currentUserIndex]->active == 1) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        currentUserContacts = p->contacts[currentUserIndex];        
    } else {
        return SOCK_RES_FAIL;
    }
    
    
    // Odoberany uzivatel 
    // Kontrola ci je meno odoberaneho uzivatela registrovane
    isValid = 0;
    int userToDeleteIndex;  // index konta odoberaneho uzivatela
    for(userToDeleteIndex = 0; userToDeleteIndex < *p->accounts_count; userToDeleteIndex++) {
        if (strcmp(p->accounts[userToDeleteIndex]->credentials->username, userToDelete) == 0) {
            isValid = 1;
            break;
        }
    }
    
    if (isValid == 1) {
        userToDeleteContacts = p->contacts[userToDeleteIndex];        
    } else {
        return SOCK_RES_FAIL;
    }
    pthread_mutex_unlock(p->accounts_mutex);
    
    
    // Odobratie z kontaktov (posunutie pamate)
    memmove(currentUserContacts->contacts[currentUserIndex],
            currentUserContacts->contacts[currentUserIndex] + 1,
            *currentUserContacts->contacts_count - currentUserIndex);
    
    memmove(userToDeleteContacts->contacts[userToDeleteIndex],
            userToDeleteContacts->contacts[userToDeleteIndex] + 1,
            *userToDeleteContacts->contacts_count - userToDeleteIndex);
    
    *currentUserContacts->contacts_count--;
    *userToDeleteContacts->contacts_count--;
    
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
    addMessageCode(buffer, messageCode);
    
    int n = write(*(p->client_sock), buffer, strlen(buffer) + 1);
    if (n < 0)
    {
        perror("Error writing to socket");
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
        } else if (messageCode == SOCK_REQ_ADD_CONTACT) {
            char* userToAdd = getSecondBufferArgument(buffer);
            clientAddNewContactHandler(p, buffer, username, userToAdd);
        } else if (messageCode == SOCK_REQ_DELETE_CONTACT) {
            char* userToDelete = getSecondBufferArgument(buffer);
            clientDeleteContactHandler(p, buffer, username, userToDelete);
        } else if (messageCode == SOCK_REQ_GET_CONTACTS) {
            clientGetContactsHandler(p, buffer, username);
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