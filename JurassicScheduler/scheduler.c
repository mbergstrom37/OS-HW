#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int minimumIndex(int arr[], int size, int used[], int usedSize);
int isInArray(int value, int arr[], int size);

int minimumIndex(int arr[], int size, int used[], int usedSize) {
	int min = arr[0];
	int index = 0;
	while(isInArray(index, used, usedSize)) {
		index++;
		min = arr[index];
	}

	for(int i = 1; i < size; i++) {
		if(arr[i] < min && !isInArray(i, used, usedSize)) {
			min = arr[i];
			index = i;
		}
	}

	return index;
}

int isInArray(int value, int arr[], int size) {
	for(int i = 0; i < size; i++) {
		if(arr[i] == value) {
			return 1;
		}
	}

	return 0;
}

int main() {
	FILE *fp = fopen("text.txt", "r");
	char ch[100];
	char lines[5][100];
	char names[5][100];
	int burstTimes[5];
	int arrivalTimes[5];
	int fifo[5];
	int totalTime = 0;
	int fifoWaitTime = 0;
	int prevFifoWaitTime;

	if(fp == NULL) {
		printf("Unable to open file.\n");
		return 1;
	}
	int i = 0;
	while(i < 5 && fgets(ch, sizeof(ch), fp)) {
		strncpy(lines[i], ch, sizeof(lines[i]) - 1);
		lines[i][sizeof(lines[i]) - 1] = '\0';
		i++;
	}
	int lineCount = i;
	char *token;

	for(int i = 0; i < lineCount; i++) {
		token = strtok(lines[i], "\t\n");
		if(token == NULL) {
			continue;
		}
		strncpy(names[i], token, sizeof(names[i]) - 1);
		names[i][sizeof(names[i]) - 1] = '\0';
		int j = 0;
		while((token = strtok(NULL, "\t\n")) != NULL) {
			if(token == NULL) {
				break;
			}
			if(j == 0) {
				burstTimes[i] = atoi(token);
				totalTime = totalTime + burstTimes[i];
				j++;
			}
			else if(j == 1) {
				arrivalTimes[i] = atoi(token);
				j++;
			}
		}
	}

	for(int i = 0; i < lineCount; i++) {
		fifo[i] = minimumIndex(arrivalTimes, lineCount, fifo, i);
	}

	for(int i = 0; i < lineCount; i++) {
		printf("%s:\t", names[fifo[i]]);
		prevFifoWaitTime = fifoWaitTime;
		for(int j = 0; j < fifoWaitTime + burstTimes[fifo[i]]; j++) {
			if(fifoWaitTime > 0) {
				printf("_");
				fifoWaitTime--;
			}
			else {
				printf("#");
			}
		}
		fifoWaitTime = burstTimes[fifo[i]] + prevFifoWaitTime;
		printf("\n");
	}

	fclose(fp);

	return 0;
}
