readme_content = """# Symulator Wieży Kontroli Lotów (Airport Control Tower Simulator)

Kompleksowy symulator środowiska lotniskowego w architekturze **Klient-Serwer** zrealizowany w języku C z wykorzystaniem mechanizmów systemowych standardu **POSIX**. Projekt demonstruje praktyczne zastosowanie zarządzania procesami, komunikacji sieciowej przez gniazda strumieniowe, wielowątkowości oraz synchronizacji dostępu do zasobów krytycznych (wzajemne wykluczanie).

Projekt został zaprojektowany w sposób modularny i wieloplikowy, zgodnie z najlepszymi praktykami inżynierii oprogramowania (separacja interfejsu od implementacji w plikach nagłówkowych, brak antywzorca dołączania plików `*.c` dyrektywą `#include`).

---

## 1. Struktura Projektu i Moduły

Aplikacja składa się z następujących plików źródłowych i konfiguracyjnych:

* **`lotnisko.h`** – Plik nagłówkowy (interfejs projektu). Zawiera definicje stałych globalnych (np. port sieciowy `8080`, rozmiar bufora danych `80`), definicję struktury `komunikat` używanej do transmisji sieciowej oraz prototypy funkcji eksportowanych przez poszczególne moduły (`uruchom_wieze()`, `uruchom_samolot()`).
* **`main.c`** – Główny moduł koordynujący symulację. Odpowiada za inicjalizację środowiska, powołanie procesów potomnych (serwera i klientów) oraz synchronizację momentu ich zakończenia.
* **`wieza.c`** – Implementacja logiczna wieży kontroli lotów działającej jako sieciowy serwer TCP. Zarządza nasłuchiwaniem na gnieździe, asynchronicznym tworzeniem wątków dla nadlatujących samolotów oraz kontrolą dostępu do pasa startowego.
* **`samolot.c`** – Implementacja logiczna pojedynczego samolotu działającego jako klient sieciowy. Odpowiada za generowanie żądania podejścia do lądowania, nawiązywanie połączenia strumieniowego oraz odbiór potwierdzenia.
* **`Makefile`** – Plik automatyzacji budowania projektu (przyrostowa kompilacja modułów obiektowych `.o` i linkowanie bibliotek systemowych).

---

## 2. Zastosowane Mechanizmy Systemowe (POSIX)

### A. Zarządzanie Procesami (`main.c`)
Symulacja operuje na poziomie niezależnych procesów systemowych odseparowanych w pamięci. Za pomocą systemowego wywołania `fork()` proces macierzysty dzieli się na:
1.  **Proces Wieży (Serwer):** Autonomiczny proces wykonujący nieskończoną pętlę nasłuchu sieciowego.
2.  **Procesy Samolotów (Klienci):** Dynamicznie generowane procesy, z których każdy reprezentuje osobną fizyczną maszynę w powietrzu.

Synchronizacja procesów realizowana jest w procesie głównym za pomocą funkcji `wait(NULL)`. Program blokuje swoje wyjście, dopóki wszystkie procesy samolotów nie zakończą pomyślnie swoich procedur lądowania, co zapobiega powstawaniu procesów-zombie.

### B. Komunikacja Sieciowa przez Gniazda TCP (`wieza.c` & `samolot.c`)
Komunikacja między procesami odbywa się za pomocą gniazd strumieniowych z rodziny `AF_INET` i typu `SOCK_STREAM`. Protokół TCP gwarantuje bezbłędne, sekwencyjne dostarczenie komunikatów w warstwie transportowej.
* **Serwer (Wieża)** wykonuje sekwencję: `socket()` (utworzenie gniazda) &rarr; `bind()` (powiązanie z portem `8080`) &rarr; `listen()` (przejście w stan nasłuchu) &rarr; `accept()` (akceptacja nadchodzącego połączenia).
* **Klient (Samolot)** inicjuje aktywne połączenie za pomocą funkcji `connect()` na adres pętli zwrotnej (`127.0.0.1`). Wymiana danych odbywa się za pomocą niskopoziomowych wywołań `read()` i `write()`.

### C. Wielowątkowość i Wątki Odczepione (`wieza.c`)
Aby zapewnić asynchroniczność i odporność na blokowanie, wieża kontrolna po zaakceptowaniu połączenia funkcją `accept()` nie przetwarza żądania w swoim głównym wątku. Zamiast tego za pomocą `pthread_create()` powołuje do życia dedykowany wątek roboczy do obsługi tej konkretnej maszyny. 

Zastosowanie funkcji `pthread_detach()` sprawia, że wątek przechodzi w stan "odczepiony" (detached) – po zakończeniu obsługi lądowania i zamknięciu gniazda, jego zasoby pamięciowe są automatycznie i natychmiastowo zwalniane przez jądro systemu, bez potrzeby wywoływania `pthread_join()`.

### D. Wzajemne Wykluczanie / Mutex (`wieza.c`)
Pas startowy lotniska stanowi **zasób krytyczny**. Jednoczesne lądowanie dwóch maszyn oznaczałoby katastrofę. Dostęp do sekcji krytycznej kontrolowany jest przez mechanizm blokady wzajemnego wykluczania `pthread_mutex_t pas_startowy`.
* Przed rozpoczęciem manewru lądowania, wątek obsługujący dany samolot musi atomowo zablokować mutex za pomocą `pthread_mutex_lock()`.
* Jeśli pas jest zajęty, wątek zostaje zawieszony przez planer systemu operacyjnego i czeka w kolejce.
* Podczas manewru wywoływana jest funkcja `sleep(2)`, która symuluje czas fizycznego lądowania i kołowania.
* Po opuszczeniu pasa wątek wywołuje `pthread_mutex_unlock()`, co budzi kolejną oczekującą w kolejce maszynę.

---

## 3. Wymagania Środowiskowe

Projekt wymaga środowiska zgodnego ze standardem **POSIX** (Linux / macOS / Unix). Z racji wykorzystania specyficznych mechanizmów jądra Linux, kompilacja na systemie Windows nie powiedzie się natywnie bez odpowiedniej warstwy emulacji.

**Rekomendowane środowisko:**
* System operacyjny: Dowolna dystrybucja systemu Linux (np. Ubuntu, Debian) lub **WSL (Windows Subsystem for Linux)** w środowisku Windows.
* Kompilator: `gcc` (GNU Compiler Collection).
* Narzędzia: `make`.

---

## 4. Instrukcja Kompilacji i Uruchomienia

Dzięki wykorzystaniu pliku `Makefile`, proces zarządzania projektem został w pełni zautomatyzowany. Flagi kompilacji zawierają `-Wall -Wextra` dla rygorystycznej kontroli błędów oraz flagę `-pthread` wymaganą do poprawnego zlinkowania systemowej biblioteki wątków `libpthread`.

Otwórz terminal w katalogu projektu i użyj jednego z poniższych poleceń:

### Kompilacja projektu
Kompiluje przyrostowo zmienione moduły do plików obiektowych `.o`, a następnie linkuje je w gotowy plik wykonywalny: