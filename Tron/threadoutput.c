#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void *threadP(void *param);
int main(int argc, char*argv[]) {
	srand(time(NULL));
	pthread_t workers[10];
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	for(int i = 0; i < 10; i++) {
		pthread_create(&workers[i],&attr,threadP,NULL);
	}

	for(int i = 0; i < 10; i++) {
		pthread_join(workers[i],NULL);
	}
}

void *threadP(void *param) {
	char msg[] = "Hello World\0";
	int count = 0;
	do {
		printf("%c",msg[count]);
		fflush(stdout);

		sleep(rand()%3);
		count++;
	} while(msg[count] != '\0');
	pthread_exit(0);
}
