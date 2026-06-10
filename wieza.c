#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lotnisko.h"

pthread_mutex_t pas_startowy; /* */

/* funkcja obsługi pojedynczego samolotu w osobnym wątku */
void* obsluga_samolotu(void* arg) {
    int gniazdo_klienta = *(int*)arg;
    free(arg);
    char bufor[1024];

    read(gniazdo_klienta, bufor, sizeof(bufor)-1); /* */
    printf("[WIEŻA] Otrzymano komunikat: %s\n", bufor);

    /* zabezpieczenie dostępu do pasa */
    pthread_mutex_lock(&pas_startowy);
    printf("[WIEŻA] Pas zablokowany. Samolot ląduje...\n");
    sleep(2); 
    printf("[WIEŻA] Pas zwolniony.\n");
    pthread_mutex_unlock(&pas_startowy);

    char odp[] = "Zezwolenie na lądowanie potwierdzone.";
    write(gniazdo_klienta, odp, sizeof(odp));
    close(gniazdo_klienta);

    return NULL;
}

void uruchom_wieze() {
    pthread_mutex_init(&pas_startowy, NULL); /* */
    
    int serwer = socket(AF_INET, SOCK_STREAM, 0); /* */
    int opt = 1;
    setsockopt(serwer, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in adres;
    adres.sin_family = AF_INET;
    adres.sin_addr.s_addr = INADDR_ANY;
    adres.sin_port = PORT;

    bind(serwer, (struct sockaddr*)&adres, sizeof(adres)); /* */
    listen(serwer, 5);
    printf("[WIEŻA] System uruchomiony. Nasłuch na porcie %d...\n", PORT);

    /* Nieskończona pętla akceptująca nadlatujące samoloty */
    while (1) {
        int gniazdo_klienta = accept(serwer, (struct sockaddr*)NULL, NULL); /* */
        if (gniazdo_klienta >= 0) {
            pthread_t watek;
            int *arg = malloc(sizeof(int));
            *arg = gniazdo_klienta;
            pthread_create(&watek, NULL, obsluga_samolotu, arg); /* */
            pthread_detach(watek); /* Zwalnianie zasobów po zakończeniu wątku */
        }
    }
    close(serwer);
    pthread_mutex_destroy(&pas_startowy);
}