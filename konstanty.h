// KONSTANTY A NASTAVENIA

#ifndef _KONSTANTY_INCLUDED
#define _KONSTANTY_INCLUDED

#include <signal.h>
#include <pthread.h>

// Ladenie
const int DEBUG_MODE = 1;                   // Mod pre vyvoj a ladenie

// VSEOBECNE
const int SOCK_BUFFER_LENGTH = 256;         // Dlzka buffera ktory sa posiela socketmi
const char SOCK_SPECIAL_SYMBOL = '~';       // Specialny znak, ktorym sprava zacina a oddeluje kod spravy a telo spravy
const int SOCK_MESSAGE_CODE_MAX_LENGTH = 3; // Maximalna dlzka ciselneho kodu spravy
const int SOCK_MESSAGE_LENGTH = 250;        // Maximalna dlzka spravy v buffri (bez ciselneho kodu spravy)
const int CLIENT_INITIAL_COUNT = 255;       // Pocet klientov, ktory je mozne obsluzit
const int CLIENT_MAX_ACCOUNT_COUNT = 255;   // Maximalny pocet uzivatelskych uctov, ktore sa mozu zaregistrovat
const int CLIENT_MESSAGE_LENGTH = 150;      // Maximalna dlzka textovej spravy, ktoru je mozne poslat inemu uzivatelovi
const int CLIENT_MAX_RECEIVED_MESSAGES_COUNT = 10;  // Maximalny pocet sprav ktore je mozne naraz poslat zo servera
const int USER_USERNAME_MIN_LENGTH = 3;     // Minimalna dlzka uzivatelskeho mena
const int USER_USERNAME_MAX_LENGTH = 30;    // Maximalna dlzka uzivatelskeho mena
const int USER_PASSWORD_MIN_LENGTH = 6;     // Minimalna dlzka uzivatelskeho hesla
const int USER_PASSWORD_MAX_LENGTH = 255;   // Maximalna dlzka uzivatelskeho hesla
const int SERVER_MAX_MESSAGES_COUNT = 1000; // Maximalny pocet sprav ulozenych na servery

const int CLIENT_STATUS_EXIT = 1;
const int CLIENT_STATUS_UNAUTHENTICATED = 2;
const int CLIENT_STATUS_AUTHENTICATED = 3;


// SOCKETY (ciselne kody)
// Spravy odosielane klientom (poziadavka - request)
const int SOCK_REQ_REGISTER = 2;            // Poziadavka na registraciu noveho uzivatela
const int SOCK_REQ_LOGIN = 3;               // Poziadavka na prihlasenie uzivatela
const int SOCK_REQ_LOGOUT = 4;              // Poziadavka na odhlasenie uzivatela
const int SOCK_REQ_ADD_CONTACT = 5;         // Poziadavka na pridanie ineho uzivatela medzi kontakty
const int SOCK_REQ_DELETE_CONTACT = 6;      // Poziadavka na odobratie uzivatela z kontaktov
const int SOCK_REQ_GET_CONTACTS = 7;        // Poziadavka na zobrazenie kontaktov prihlaseneho uzivatela
const int SOCK_REQ_SEND_MESSAGE = 8;        // Poziadavka na poslanie spravy inemu uzivatelovi
const int SOCK_REQ_GET_RECENT_MESSAGES = 9; // Poziadavka na zobrazenie neprecitanych sprav od daneho uzivatela
const int SOCK_REQ_DELETE_ACCOUNT = 10;     // Poziadavka na vymazanie konta uzivatela

// ...

// Spravy odosielane serverom (odpoved - response)
const int SOCK_RES_ERROR = 100;             // Vseobecny chybovy stav
const int SOCK_RES_OK = 101;                // Vseobecny stav uspechu
const int SOCK_RES_FAIL = 102;              // Vseobecny stav neuspechu

// ...


// DATOVE STRUKTURY

/**
 * Prihlasovacie alebo registracne udaje konta uzivatela
 */
typedef struct {
    char* username;
    char* password;
} ACCOUNT_CREDENTIALS;


/**
 * Struktura obsahuje konto uzivatela a stav ci je aktivne (nie je vymazane)
 */
typedef struct {
    ACCOUNT_CREDENTIALS* credentials;
    int** contacts;    // Pole rovnakej dlzky ako je mozny pocet registrovanych uzivatelov s cislami 1 a 0 (je kontakt / nie je kontakt) 
    int* active;    // priznak 0/1
} USER_ACCOUNT;


/**
 * Sprava inemu uzivatelovi
 */
typedef struct {
    char* sender;                       // Meno odosielatela
    char* recipient;                    // Meno prijemcu
    char* text;                         // Text spravy
    int* unread;                        // Priznak ci bola precitana
} MESSAGE;


/**
 * Klientsky socket
 */
typedef struct {
    int* client_sock;                   // Socket pripojeneho klienta
    
    USER_ACCOUNT** accounts;            // Pole vsetkych zaregistrovanych uzivatelskych kont
    int* accounts_count;                // Pocet platnych prvkov (kont)
    pthread_mutex_t* accounts_mutex;    // mutex pre pristup k accounts
    
    MESSAGE** messages;                 // Pole nedorucenych sprav
    int* messages_count;                // Pocet platnych prvkov (sprav)
    pthread_mutex_t* messages_mutex;    // mutex pre pristup k messages
} CLIENT_SOCKET;


#endif
