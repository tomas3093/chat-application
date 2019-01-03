Chat app
====

## Dokumentacia
---

### Sockety

#### Client - spravy odosielane na server:
  * prihlasovacie udaje (prihlasenie)
  * prihlasovacie udaje (registracia)
  * poziadavka na zacatie chatu s konkretnym uzivatelom
  * text spravy inemu uzivatelovi
  * poziadavka na zobrazenie vsetkych registrovanych uzivatelov
  * poziadavka na zobrazenie kontaktov
  * poziadavka na odstranenie kontaktu

#### Server - spravy odosielane klientovi:
  * odpoved na prihlasenie (OK / FAIL)
  * odpoved na registraciu (OK / FAIL)
  * upozornenie uzivatela ze si ho iny chce pridat do kontaktov
  * upozornenie pouzivatela ze si ho iny odstranil z kontaktov a odstranenie toho uzivatela aj z jeho kontaktov

#### Vzorova sprava
[Kod spravy][Specialny oddelovaci znak][Telo spravy]

#### Struktura sprav
    * [SOCK_REQ_REGISTER][SOCK_SPECIAL_SYMBOL][LOGIN][SOCK_SPECIAL_SYMBOL[]PASSWORD]
    * [SOCK_REQ_LOGIN][SOCK_SPECIAL_SYMBOL][LOGIN][SOCK_SPECIAL_SYMBOL][PASSWORD]
    * [SOCK_REQ_ADD_CONTACT][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME]
    * [SOCK_REQ_DELETE_CONTACT][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME]
    * [SOCK_REQ_GET_CONTACTS][SOCK_SPECIAL_SYMBOL]
    * [SOCK_REQ_SEND_MESSAGE][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME][SOCK_SPECIAL_SYMBOL][MESSAGE_TEXT]
    * [SOCK_REQ_GET_UNREAD_MESSAGES][SOCK_SPECIAL_SYMBOL][CONTACT_USERNAME]
    * ...

Odpovede na requesty su vzdy iba uspech (SOCK_RES_OK) alebo neuspech (SOCK_RES_FAIL)

#### Specialne pripady   
    * response s kontaktmi uzivatela            - [SOCK_RES_OK][SOCK_SPECIAL_SYMBOL][1. CONTACT_USERNAME][SOCK_SPECIAL_SYMBOL][2. CONTACT_USERNAME][SOCK_SPECIAL_SYMBOL] ...
    * response so spravou od ineho uzivatela    - po requeste server posiela postupne jednotlive spravy a skonci spravou s kodom SOCK_RES_FAIL

---
### Zodpovednosti

#### Server
  * pameta si vsetkych zaregistrovanych uzivatelov
  * pameta si kto je s kym kontakt
  * vie kto je aktualne prihlaseny
  * pameta si vsetky nedorucene spravy (neuchovava historiu sprav)

#### Client
  * vie svoje meno (nick)
  * vie s kym aktualne komunikuje
  * ...

---
### Struktury
    * CLIENT_SOCKET
    * ACCOUNT_CREDENTIALS
    * USER_ACCOUNT
    * ...

#### Data na serveri
    * int sockfd - socket servera (file descriptor)
    * USER_ACCOUNT* accounts[CLIENT_MAX_ACCOUNT_COUNT] - pole vsetkych zaregistrovanych kont

#### Data u klienta
    * username - meno prihlaseneho uzivatela
    * ...


---
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
  * dorobit zobrazenie neprecitanych sprav
  * dorobit posielanie sprav
  * nefunguje odstranovanie uzivatelov z kontaktov
  * pridat vymazanie uctu uzivatela (iba sa zmeni 'active' na hodnotu 0 v USER_ACCOUNT)
  * pridat LOGOUT
  * ... 

---
### Sources
  * https://www.binarytides.com/server-client-example-c-sockets-linux/
