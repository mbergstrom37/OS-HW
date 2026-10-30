all: scheduler

scheduler: scheduler.o
	gcc scheduler.o -o scheduler

scheduler.o: scheduler.c
	gcc -c scheduler.c

clean:
	rm scheduler.o scheduler

install:
	cp scheduler /usr/local/bin

uninstall:
	rm /usr/local/bin/scheduler
