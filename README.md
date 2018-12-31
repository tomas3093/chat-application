Chat app
====

## Dokumentacia
---

### Sockety

#### Client - spravy odosielane na server:
  * prihlasovacie udaje (prihlasenie)
  * prihlasovacie udaje (registracia)
  * text spravy inemu uzivatelovi
  * poziadavka na zacatie chatu s konkretnym uzivatelom
  * poziadavka na zobrazenie vsetkych registrovanych uzivatelov
  * poziadavka na zobrazenie kontaktov
  * poziadavka na odstranenie kontaktu

#### Server - spravy odosielane klientovi:
  * odpoved na prihlasenie (OK/ NO OK)
  * odpoved na registraciu (ci je unikatny login)
  * upozornenie uzivatela ze si ho iny chce pridat do kontaktov
  * upozornenie pouzivatela ze si ho iny odstranil z kontaktov a odstranenie toho uzivatela aj z jeho kontaktov

#### Vzorova sprava
[Kod spravy][Specialny oddelovaci znak][Telo spravy]

#### Struktura sprav
    * [SOCK_REQ_CONNECT][SOCK_SPECIAL_SYMBOL][]
    * [SOCK_REQ_DISCONNECT][SOCK_SPECIAL_SYMBOL][]
    * [SOCK_REQ_REGISTER][SOCK_SPECIAL_SYMBOL][(LOGIN)(SOCK_SPECIAL_SYMBOL)(PASSWORD)]
    * ...

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
    * int sockfd - socket servera
    * int client_sockets[CLIENT_INITIAL_COUNT] - pole vsetkych klientskych soketov
    * pthread_t client_threads[CLIENT_INITIAL_COUNT] - pole vsetkych klientskych vlakien
    * USER_ACCOUNT accounts[CLIENT_MAX_ACCOUNT_COUNT] - pole vsetkych zaregistrovanych kont

#### Data u klienta
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
