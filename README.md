Chat app
====

## Dokumentacia
---

### Sockety

#### Client - spravy odosielane na server:
* [SOCK_REQ_REGISTER]             - poziadavka na registraciu
* [SOCK_REQ_LOGIN]                - poziadavka na prihlasenie
* [SOCK_REQ_ADD_CONTACT]          - poziadavka na pridanie noveho kontaktu  
* [SOCK_REQ_DELETE_CONTACT]       - poziadavka na zmazanie kontaktu
* [SOCK_REQ_GET_CONTACTS]         - poziadavka na zobrazenie kontaktov
* [SOCK_REQ_START_CHAT]           - poziadavka na zacatie chatu s konkretnym uzivatelom
* [SOCK_REQ_SEND_MESSAGE]         - poziadavka na poslanie spravy inemu uzivatelovi
* [SOCK_REQ_GET_RECENT_MESSAGES]  - poziadavka na zobrazenie najnovsich sprav z chatu s danym uzivatelom
* [SOCK_REQ_DELETE_ACCOUNT]       - poziadavka na zmazanie konta uzivatela

#### Server - spravy odosielane klientovi:
* odpoved (OK / FAIL)

Odpovede na requesty su vzdy iba uspech (SOCK_RES_OK) alebo neuspech (SOCK_RES_FAIL), okrem specialnych pripadov - vid. nizsie

#### Specialne pripady   
* response s kontaktmi uzivatela            - [SOCK_RES_OK][SOCK_SPECIAL_SYMBOL][1. CONTACT_USERNAME][SOCK_SPECIAL_SYMBOL][2. CONTACT_USERNAME]...
* response so spravou od ineho uzivatela    - po requeste server posiela postupne jednotlive spravy a skonci spravou s kodom SOCK_RES_FAIL

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
* CLIENT_SOCKET         - obsahuje data, ktore sa zdielaju medzi jednotlivymi vlaknami, t.j file descriptor socketu, pole s uzivatelmi, pole so spravami a synchronizacne prostriedky
* ACCOUNT_CREDENTIALS   - 
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
Client-server aplikácia, ktorá umožní komunikovať ľubovoľnému počtu používateľov.

#### Aplikácia umožní používateľovi
* vytvoriť/zrušiť účet,
* prihlásiť sa/odhlásiť sa,
* pridať si iného používateľa do svojich kontaktov – pridávaný používateľ o tom musí byť informovaný a musí s tým súhlasiť,
* odstrániť iného používateľa zo svojich kontaktov – odstraňovaný používateľa o tom musí byť informovaný a z jeho kontaktov musí byť automaticky odstránený používateľ, ktorý si ho odstránil,
* poslať textovú správu inému používateľovi, ktorého má vo svojich kontaktoch (ak je tento odhlásený, musí dostať správu hneď, ako sa prihlási).


---
### Known bugs and TODOs
  * manualne skontrolovat memory leaky ((?) alebo cez nejaky tool) - manualne som nenasiel ziadny leak
  * napisat vlastny makefile
  * napisat dokumentaciu:
    * struktura projektu
        * server.c - obsahuje main funkciu, v ktorej sa prijimaju spojenia od klientov a pre kazde sa vytvara samostatne vlakno
        * server.h - obsahuje vsetky funkcionality, ktore su potrebne pre obsluhu poziadaviek klientov (registracia, prihlasenie, poslanie spravy...)
        * client.c - obsahuje hlavnu slucku, pripoji sa k serveru a zabezpecuje uzivatelske rozhranie (prijima vstupy od uzivatela a vola prislusne funkcie)
        * client.h - obsahuje funkcie, ktore sa volaju z hlavnej slucky programu
        * konstanty.h - obsahuje vsetky konstanty a datove struktury, ktore pouziva aplikacia
        * routines.h - obsahuje pomocne funkcie, ktore pouziva server aj client

    * pouzitie socketov - server pocuva na danom porte (listen), po pripojeni klienta (accept) zacne komunikacia (funkcie read, write, recv)
    * pouzitie vlakien - na serveri bezi multithreading, t.j. po pripojeni klienta a vytvori vlakno a v nom bezi obsluha daneho klienta; hlavne vlakno na serveri nikdy neskonci, pre skoncenie sa musi killnut
    * synchronizacny problem
        * jednotlive vlakna na serveri pristupuju k zdielanym strukturam, ktore obsahuju spravy (MESSAGE* messages[]) a registrovanych uzivatelov (USER_ACCOUNT* accounts)
        * tuto synchronizaciu sme riesili vzajomnym vylucenim pomocou mechanizmu mutexov
    * ine problemy, ktore sme riesili - problem bol skoro vsetko :D 

---
### Sources
  * https://www.binarytides.com/server-client-example-c-sockets-linux/
