// KONSTANTY A NASTAVENIA

#ifndef _KONSTANTY_INCLUDED
#define _KONSTANTY_INCLUDED

// VSEOBECNE
const int SOCK_MESSAGE_BUFFER_LENGTH = 256; // Dlzka buffera ktory sa posiela socketmi
const char SOCK_SPECIAL_SYMBOL = '~';       // Specialny znak, ktorym sprava zacina a oddeluje kod spravy a telo spravy
const int SOCK_MESSAGE_CODE_MAX_LENGTH = 3; // Maximalna dlzka ciselneho kodu spravy
const int CLIENT_INITIAL_COUNT = 255;       // Pocet klientov, ktory je mozne obsluzit
const int USER_USERNAME_MAX_LENGTH = 30;    // Maximalna dlzka uzivatelskeho mena
const int USER_PASSWORD_MAX_LENGTH = 255;    // Maximalna dlzka uzivatelskeho hesla


// SOCKETY (ciselne kody)
// Spravy odosielane klientom (poziadavka - request)
const int SOCK_REQ_CONNECT = 0;             // Poziadavka na pripojenie (prva sprava z klienta na server)
const int SOCK_REQ_DISCONNECT = 1;          // Oznamenie o odpojeni klienta
const int SOCK_REQ_REGISTER = 2;            // Poziadavka na registraciu noveho uzivatela
const int SOCK_REQ_LOGIN = 3;               // Poziadavka na prihlasenie uzivatela
const int SOCK_REQ_LOGOUT = 4;              // Poziadavka na odhlasenie uzivatela

// ...

// Spravy odosielane serverom (odpoved - response)
const int SOCK_RES_CONNECT = 100;           // Uspesne pripojenie
const int SOCK_RES_DISCONNECT = 101;        // Uspesne odpojenie
const int SOCK_RES_REGISTER_OK = 102;       // Uspesna registracia
const int SOCK_RES_REGISTER_FAIL = 103;     // Neuspesna registracia

// ...


// Struktury

/**
 * Klientsky socket
 */
typedef struct {
    int newsockfd;
} CLIENT_SOCKET;


/**
 * Prihlasovacie alebo registracne udaje konta uzivatela
 */
typedef struct {
    char* username;
    char* password;
} ACCOUNT_CREDENTIALS;


#endif
