// KONSTANTY A NASTAVENIA

#ifndef _KONSTANTY_INCLUDED
#define _KONSTANTY_INCLUDED

#include <signal.h>
#include <pthread.h>


// VSEOBECNE
const int SOCK_BUFFER_LENGTH = 256;         // Dlzka buffera ktory sa posiela socketmi
const char SOCK_SPECIAL_SYMBOL = '~';       // Specialny znak, ktorym sprava zacina a oddeluje kod spravy a telo spravy
const int SOCK_MESSAGE_CODE_MAX_LENGTH = 3; // Maximalna dlzka ciselneho kodu spravy
// Dlzka samotnej spravy (messageText) ktory sa posiela socketmi
//const int SOCK_MESSAGE_LENGTH = SOCK_BUFFER_LENGTH - SOCK_MESSAGE_CODE_MAX_LENGTH - 3;
const int SOCK_MESSAGE_LENGTH = 250;
const int CLIENT_INITIAL_COUNT = 255;       // Pocet klientov, ktory je mozne obsluzit
const int CLIENT_MAX_ACCOUNT_COUNT = 255;   // Maximalny pocet uzivatelskych uctov, ktore sa mozu zaregistrovat
const int CLIENT_MAX_CONTACTS_COUNT = 20;   // Maximalny pocet kontaktov, ktore moze mat 1 uzivatel
const int USER_USERNAME_MIN_LENGTH = 3;     // Minimalna dlzka uzivatelskeho mena
const int USER_USERNAME_MAX_LENGTH = 30;    // Maximalna dlzka uzivatelskeho mena
const int USER_PASSWORD_MIN_LENGTH = 6;     // Minimalna dlzka uzivatelskeho hesla
const int USER_PASSWORD_MAX_LENGTH = 255;   // Maximalna dlzka uzivatelskeho hesla


// SOCKETY (ciselne kody)
// Spravy odosielane klientom (poziadavka - request)
const int SOCK_REQ_CONNECT = 0;             // Poziadavka na pripojenie (prva sprava z klienta na server)
const int SOCK_REQ_DISCONNECT = 1;          // Oznamenie o odpojeni klienta
const int SOCK_REQ_REGISTER = 2;            // Poziadavka na registraciu noveho uzivatela
const int SOCK_REQ_LOGIN = 3;               // Poziadavka na prihlasenie uzivatela
const int SOCK_REQ_LOGOUT = 4;              // Poziadavka na odhlasenie uzivatela

// ...

// Spravy odosielane serverom (odpoved - response)
const int SOCK_RES_ERROR = 200;             // Vseobecny chybovy stav
const int SOCK_RES_OK = 201;                // Vseobecny stav uspechu
const int SOCK_RES_FAIL = 202;              // Vseobecny stav neuspechu

const int SOCK_RES_CONNECT = 100;           // Uspesne pripojenie
const int SOCK_RES_DISCONNECT = 101;        // Uspesne odpojenie

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
    int active;     // priznak 0/1
} USER_ACCOUNT;


/**
 * Kontakty uzivatela
 */
typedef struct {
    USER_ACCOUNT** contacts;            // Pole kontaktov
} USER_CONTACTS;


/**
 * Klientsky socket
 */
typedef struct {
    int* client_sock;                   // Socket pripojeneho klienta
    
    USER_ACCOUNT** accounts;            // Pole vsetkych zaregistrovanych uzivatelskych kont
    int* accounts_count;                // Pocet platnych prvkov (kont)
    pthread_mutex_t* accounts_mutex;    // mutex pre pristup k accounts
    
    USER_CONTACTS** contacts;           // Pole uzivatelskych kontaktov
} CLIENT_SOCKET;


#endif
