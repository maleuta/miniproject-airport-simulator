#ifndef LOTNISKO_H
#define LOTNISKO_H

#define PORT 8080
#define ROZMIAR 80

struct komunikat {
    long typ;
    char tekst[ROZMIAR];
};

void uruchom_wieze();
void uruchom_samolot(int id_samolotu);

#endif