#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "lotnisko.h"

int main() {
    printf("--- Start Symulacji Lotniska ---\n");

    /* tworzenie procesu głównego serwera (Wieży) */
    if (fork() == 0) {
        uruchom_wieze();
        exit(0);
    }

    sleep(1); 

    /* generowanie procesów potomnych - samolotów */
    for (int i = 1; i <= 5; i++) {
        if (fork() == 0) {
            uruchom_samolot(i);
            exit(0);
        }
        sleep(1); /* symulacja odstępów między przylotami */
    }

    /* oczekiwanie na lądowanie wszystkich samolotów */
    for (int i = 0; i < 5; i++) {
        wait(NULL); /* służy do synchronizacji */
    }

    printf("Wszystkie samoloty wylądowały. Możesz zamknąć serwer (Ctrl+C).\n");
    
    wait(NULL); /* służy do synchronizacji */
    return 0;
}