// KONSTANTY A NASTAVENIA

#ifndef _KONSTANTY_INCLUDED
#define _KONSTANTY_INCLUDED

// Vseobecne
const int SOCK_BUFFER_LENGTH = 256;     // Dlzka buffera ktory sa posiela socketmi
const char SOCK_SPECIAL_SYMBOL = '~';   // Specialny znak, ktorym sprava zacina a oddeluje kod spravy a telo spravy
const int CLIENT_INITIAL_COUNT = 255;   // Pocet klientov, ktory je mozne obsluzit


// Sockety
// Spravy odosielane klientom (poziadavka - request)
const char* SOCK_REQ_CONNECT = "0";         // Poziadavka na pripojenie (prva sprava z klienta na server)
const char* SOCK_REQ_DISCONNECT = "1";          // Oznamenie o odpojeni klienta
const char* SOCK_REQ_REGISTRATION = "2";    // Poziadavka na registraciu noveho uzivatela
const char* SOCK_REQ_LOGIN = "3";           // Poziadavka na prihlasenie uzivatela
const char* SOCK_REQ_LOGOUT = "4";           // Poziadavka na odhlasenie uzivatela

// ...

// Spravy odosielane serverom (odpoved - response)
const char* SOCK_RES_CONNECT = "100";
const char* SOCK_RES_DISCONNECT = "101";

// ...


// Struktury

/**
 * Klientsky socket
 */
typedef struct {
    int newsockfd;
} CLIENT_SOCKET;



#endif
