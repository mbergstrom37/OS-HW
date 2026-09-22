CC = gcc
CFLAGS = -Wno-implicit-function-declaration

all: dinobash

dinobash: DinoBash.o
	$(CC) $(CFLAGS) DinoBash.o -o dinobash

DinoBash.o: DinoBash.c
	$(CC) $(CFLAGS) -c DinoBash.c

clean:
	rm DinoBash.o dinobash

install:
	cp dinobash /usr/local/bin

uninstall:
	rm /usr/local/bin/dinobash

