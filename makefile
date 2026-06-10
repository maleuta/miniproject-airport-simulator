CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = lotnisko_symulacja

OBJS = main.o wieza.o samolot.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

main.o: main.c lotnisko.h
	$(CC) $(CFLAGS) -c main.c

wieza.o: wieza.c lotnisko.h
	$(CC) $(CFLAGS) -c wieza.c

samolot.o: samolot.c lotnisko.h
	$(CC) $(CFLAGS) -c samolot.c

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)