Chat app
====

## Dokumentacia
---

### Sockety

#### Client - spravy odosielane na server:
  * poziadavka na registraciu
  * poziadavka na prihlasenie
  * poziadavka na pridanie noveho kontaktu  
  * poziadavka na zmazanie kontaktu
  * poziadavka na zobrazenie kontaktov
  * poziadavka na zacatie chatu s konkretnym uzivatelom
  * poziadavka na poslanie spravy inemu uzivatelovi
  * poziadavka na zobrazenie najnovsich sprav z chatu s danym uzivatelom
  * poziadavka na zmazanie konta uzivatela

Odpovede na requesty su vzdy iba uspech (SOCK_RES_OK) alebo neuspech (SOCK_RES_FAIL)

#### Server - spravy odosielane klientovi:
  * odpoved (OK / FAIL)

#### Vzorova sprava
[Kod spravy][Specialny oddelovaci znak][Telo spravy]

#### Struktura sprav
    * [SOCK_REQ_REGISTER][SOCK_SPECIAL_SYMBOL][LOGIN][SOCK_SPECIAL_SYMBOL][PASSWORD]
    * [SOCK_REQ_LOGIN][SOCK_SPECIAL_SYMBOL][LOGIN][SOCK_SPECIAL_SYMBOL][PASSWORD]
    * [SOCK_REQ_ADD_CONTACT][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME]
    * [SOCK_REQ_DELETE_CONTACT][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME]
    * [SOCK_REQ_GET_CONTACTS][SOCK_SPECIAL_SYMBOL]
    * [SOCK_REQ_START_CHAT][SOCK_SPECIAL_SYMBOL]
    * [SOCK_REQ_SEND_MESSAGE][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME][SOCK_SPECIAL_SYMBOL][MESSAGE_TEXT]
    * [SOCK_REQ_GET_RECENT_MESSAGES][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME]
    * [SOCK_REQ_DELETE_ACCOUNT][SOCK_SPECIAL_SYMBOL]

#### Specialne pripady   
    * response s kontaktmi uzivatela            - [SOCK_RES_OK][SOCK_SPECIAL_SYMBOL][1. CONTACT_USERNAME][SOCK_SPECIAL_SYMBOL][2. CONTACT_USERNAME][SOCK_SPECIAL_SYMBOL] ...
    * response so spravou od ineho uzivatela    - po requeste server posiela postupne jednotlive spravy a skonci spravou s kodom SOCK_RES_FAIL

---
### Zodpovednosti

#### Server
  * pameta si vsetkych zaregistrovanych uzivatelov
  * pameta si kto je s kym kontakt
  * vie kto je aktualne prihlaseny
  * pameta si vsetky spravy

#### Client
  * vie svoje prihlasovacie meno
  * vie s kym aktualne komunikuje

---
### Struktury
    * CLIENT_SOCKET
    * ACCOUNT_CREDENTIALS
    * USER_ACCOUNT
    * MESSAGE

### Konstanty
#### Ladenie
    * const int DEBUG_MODE = 0;                   // Mod pre vyvoj a ladenie

#### VSEOBECNE
    * const int SOCK_BUFFER_LENGTH = 256;         // Dlzka buffera ktory sa posiela socketmi
    * const char SOCK_SPECIAL_SYMBOL = '~';       // Specialny znak, ktorym sprava zacina a oddeluje kod spravy a telo spravy
    * const int SOCK_MESSAGE_CODE_MAX_LENGTH = 3; // Maximalna dlzka ciselneho kodu spravy
    * const int SOCK_MESSAGE_LENGTH = 250;        // Maximalna dlzka spravy v buffri (bez ciselneho kodu spravy)
    * const int CLIENT_INITIAL_COUNT = 255;       // Pocet klientov, ktory je mozne obsluzit
    * const int CLIENT_MAX_ACCOUNT_COUNT = 255;   // Maximalny pocet uzivatelskych uctov, ktore sa mozu zaregistrovat
    * const int CLIENT_MESSAGE_LENGTH = 150;      // Maximalna dlzka textovej spravy, ktoru je mozne poslat inemu uzivatelovi
    * const int CLIENT_MAX_RECEIVED_MESSAGES_COUNT = 10;  // Maximalny pocet sprav ktore je mozne naraz poslat zo servera
    * const int USER_USERNAME_MIN_LENGTH = 3;     // Minimalna dlzka uzivatelskeho mena
    * const int USER_USERNAME_MAX_LENGTH = 30;    // Maximalna dlzka uzivatelskeho mena
    * const int USER_PASSWORD_MIN_LENGTH = 6;     // Minimalna dlzka uzivatelskeho hesla
    * const int USER_PASSWORD_MAX_LENGTH = 255;   // Maximalna dlzka uzivatelskeho hesla
    * const int SERVER_MAX_MESSAGES_COUNT = 1000; // Maximalny pocet sprav ulozenych na servery

#### Stavy klienta
    * const int CLIENT_STATUS_EXIT = 1;           // Klient ukoncil aplikaciu
    * const int CLIENT_STATUS_UNAUTHENTICATED = 2;// Klient nie je prihlaseny
    * const int CLIENT_STATUS_AUTHENTICATED = 3;  // Klient je prihlaseny


#### SOCKETY (ciselne kody)
#### Spravy odosielane klientom (poziadavka - request)
    * const int SOCK_REQ_REGISTER = 2;            // Poziadavka na registraciu noveho uzivatela
    * const int SOCK_REQ_LOGIN = 3;               // Poziadavka na prihlasenie uzivatela
    * const int SOCK_REQ_LOGOUT = 4;              // Poziadavka na odhlasenie uzivatela
    * const int SOCK_REQ_ADD_CONTACT = 5;         // Poziadavka na pridanie ineho uzivatela medzi kontakty
    * const int SOCK_REQ_DELETE_CONTACT = 6;      // Poziadavka na odobratie uzivatela z kontaktov
    * const int SOCK_REQ_GET_CONTACTS = 7;        // Poziadavka na zobrazenie kontaktov prihlaseneho uzivatela
    * const int SOCK_REQ_SEND_MESSAGE = 8;        // Poziadavka na poslanie spravy inemu uzivatelovi
    * const int SOCK_REQ_GET_RECENT_MESSAGES = 9; // Poziadavka na zobrazenie neprecitanych sprav od daneho uzivatela
    * const int SOCK_REQ_DELETE_ACCOUNT = 10;     // Poziadavka na vymazanie konta uzivatela
    * const int SOCK_REQ_START_CHAT = 11;         // Poziadavka na zistenie ci je mozne zacat chat s danym uzivatelom

#### Spravy odosielane serverom (odpoved - response)
    * const int SOCK_RES_ERROR = 100;             // Vseobecny chybovy stav
    * const int SOCK_RES_OK = 101;                // Vseobecny stav uspechu
    * const int SOCK_RES_FAIL = 102;              // Vseobecny stav neuspechu


### Datove struktury
#### Data na serveri
    * int sockfd - socket servera (file descriptor)
    * int client_sock
    * USER_ACCOUNT* accounts[CLIENT_MAX_ACCOUNT_COUNT]
    * int accounts_count
    * pthread_mutex_t accounts_mutex
    * MESSAGE* messages[SERVER_MAX_MESSAGES_COUNT]
    * int messages_count
    * pthread_mutex_t messages_mutex
    
#### Data u klienta
    * int sockfd
    * char buffer[SOCK_BUFFER_LENGTH]
    * char* username - meno prihlaseneho uzivatela (nastavi sa po prihlaseni)


---
## Specifikacia

### Chat (2 študenti) 
- klient-server aplikácia, ktorá umožní komunikovať ľubovoľnému počtu používateľov.

#### Aplikácia umožní používateľovi:
  * vytvoriť/zrušiť účet,
  * prihlásiť sa/odhlásiť sa,
  * pridať si iného používateľa do svojich kontaktov – pridávaný používateľ o tom musí byť informovaný a musí s tým súhlasiť,
  * odstrániť iného používateľa zo svojich kontaktov – odstraňovaný používateľa o tom musí byť informovaný a z jeho kontaktov musí byť automaticky odstránený používateľ, ktorý si ho odstránil,
  * poslať textovú správu inému používateľovi, ktorého má vo svojich kontaktoch (ak je tento odhlásený, musí dostať správu hneď, ako sa prihlási).


---
### Known bugs and TODOs
  * manualne skontrolovat memory leaky ((?) alebo cez nejaky tool)
  * napisat dokumentaciu 

---
### Sources
  * https://www.binarytides.com/server-client-example-c-sockets-linux/
