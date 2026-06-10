#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "lotnisko.h"


/* 
Inicjuje własne gniazdo i łączy się z serwerem za pomocą funkcji connect na adresie lokalnym (127.0.0.1). 
Wymiana komunikatów ("prośba o lądowanie" -> "zezwolenie") następuje przy użyciu read i write
*/

void uruchom_samolot(int id_samolotu) {
    char bufor[1024];
    char msg[128];
    sprintf(msg, "Samolot ID:%d prosi o podejście do lądowania", id_samolotu);

    int gniazdo = socket(AF_INET, SOCK_STREAM, 0); /* */
    struct sockaddr_in adres;
    adres.sin_family = AF_INET;
    adres.sin_port = PORT;
    inet_pton(AF_INET, "127.0.0.1", &adres.sin_addr);

    if (connect(gniazdo, (struct sockaddr*)&adres, sizeof(adres)) < 0) { /* */
        printf("[SAMOLOT %d] Brak połączenia z wieżą!\n", id_samolotu);
        close(gniazdo);
        return;
    }

    write(gniazdo, msg, sizeof(msg));
    read(gniazdo, bufor, sizeof(bufor)-1); /* */
    printf("[SAMOLOT %d] Odpowiedź z wieży: %s\n", id_samolotu, bufor);
    
    close(gniazdo);
}