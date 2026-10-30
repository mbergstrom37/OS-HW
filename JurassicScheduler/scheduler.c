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
	int prevFifoWaitTime = 0;
	float avgFifoWaitTime = 0.0;
	float avgFifoResponseTime = 0.0;
	int fifoThroughput = 0;
	int currentTime = 0;
	int completeProcesses = 0;
	int fifoThroughputBool = 1;
	int currBurstTimes[5];

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

	printf("\nFIFO\n");
	for(int i = 0; i < lineCount; i++) {
		currentTime = 0;
		printf("\t%s:\t", names[fifo[i]]);
		avgFifoWaitTime += (float)fifoWaitTime - arrivalTimes[fifo[i]];
		prevFifoWaitTime = fifoWaitTime;
		for(int j = 0; j < prevFifoWaitTime + burstTimes[fifo[i]]; j++) {
			if(currentTime == 10 && fifoThroughputBool) {
				fifoThroughput = completeProcesses;
				fifoThroughputBool = 0;
			}
			if(currentTime < arrivalTimes[fifo[i]]) {
				printf(" ");
				fifoWaitTime--;
			}
			else if(fifoWaitTime > 0) {
				printf("-");
				fifoWaitTime--;
			}
			else {
				if(fifoWaitTime == 0) {
					avgFifoResponseTime += 
						((float)currentTime - (float)arrivalTimes[fifo[i]]);
				}
				printf("#");
				fifoWaitTime--;
			}
			currentTime++;
		}
		completeProcesses++;
		fifoWaitTime = burstTimes[fifo[i]] + prevFifoWaitTime;
		printf("\n");
	}

	avgFifoWaitTime /= (float)lineCount;
	avgFifoResponseTime /= (float)lineCount;
	printf("\tAverage Wait Time: %f\n", avgFifoWaitTime);
	printf("\tAverage Response Time: %f\n", avgFifoResponseTime);
	printf("\tThroughput over 10 cycles: %d\n\n", fifoThroughput);

	printf("SJF\n");
	int sjfSchedule[totalTime];
	int completed[5] = {0};

	for(int i = 0; i < lineCount; i++) {
		currBurstTimes[i] = burstTimes[i];
	}
	for(int i = 0; i < totalTime; i++) {
		sjfSchedule[i] = -1;
	}

	currentTime = 0;
	int numCompleted = 0;
	while(numCompleted < lineCount) {
		int minIndex = -1;
		int minBurst = totalTime;

		for(int i = 0; i < lineCount; i++) {
			if(!completed[i] && arrivalTimes[i] <= currentTime) {
				if(currBurstTimes[i] < minBurst) {
					minBurst = currBurstTimes[i];
					minIndex = i;
				}
			}
		}

		if(minIndex = -1) {
			currentTime++;
			continue;
		}

		sjfSchedule[currentTime] = minIndex;
		currBurstTimes[minIndex]--;

		if(currBurstTimes[minIndex] == 0) {
			completed[minIndex] = 1;
			numCompleted++;
		}

		currentTime++;
	}

	for(int i = 0; i < lineCount; i++) {
		printf("\t%s\t", names[i]);
		for(int j = 0; j < currentTime; j++) {
			if(sjfSchedule[j] == i) {
				printf("#");
			}
			else if(j < arrivalTimes[i]) {
				printf(" ");
			}
			else {
				printf("-");
			}
		}
		printf("\n");
	}

	fclose(fp);

	return 0;
}
