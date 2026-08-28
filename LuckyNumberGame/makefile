CC = gcc
CFLAGS = -Wno-implicit-function-declaration

all: game

game: main.o LuckyNumber.o
	$(CC) $(CFLAGS) main.o LuckyNumber.o -o game

main.o: main.c LuckyNumber.h Record.h
	$(CC) $(CFLAGS) -c main.c

LuckyNumber.o: LuckyNumber.c LuckyNumber.h
	$(CC) $(CFLAGS) -c LuckyNumber.c

clean:
	rm main.o LuckyNumber.o game
install:
	cp game /usr/local/bin
uninstall:
	rm /usr/local/bin/game
