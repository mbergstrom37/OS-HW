#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 5

//got queue from geeksforgeeks and tweaked to fit my needs
typedef struct {
	int items[MAX_SIZE];
	int front;
	int rear;
	int count;

} Queue;

void initQueue(Queue *q) {
	q->front = 0;
	q->rear = 0;
	q->count = 0;
}

bool isEmpty(Queue *q) {
	return q->count == 0;
}

bool isFull(Queue *q) {
	return q->rear == MAX_SIZE;
}

void enqueue(Queue *q, int value) {
	if(isFull(q)) {
		printf("Queue is full\n");
		return;
	}
	q->items[q->rear] = value;
	q->rear = (q->rear + 1) % MAX_SIZE;
	q->count++;
}

int dequeue(Queue *q) {
	if(isEmpty(q)) {
		printf("Queue is empty\n");
		return -1;
	}
	int value = q->items[q->front];
	q->front = (q->front + 1) % MAX_SIZE;
	q->count--;

	return value;
}

int peek(Queue *q) {
	if(isEmpty(q)) {
		return -1;
	}
	return q->items[q->front];
}

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

	//SJF made mostly with AI. a failed AI-less attempt was made as a basis for the generation
	printf("SJF\n");
	int sjfSchedule[totalTime + 100];
	int completed[5] = {0};

	for(int i = 0; i < lineCount; i++) {
		currBurstTimes[i] = burstTimes[i];
	}
	for(int i = 0; i < totalTime + 100; i++) {
		sjfSchedule[i] = -1;
	}

	currentTime = 0;
	int numCompleted = 0;
	while(numCompleted < lineCount) {
		int minIndex = -1;
		int minBurst = totalTime + 100;

		for(int i = 0; i < lineCount; i++) {
			if(!completed[i] && arrivalTimes[i] <= currentTime) {
				if(currBurstTimes[i] < minBurst) {
					minBurst = currBurstTimes[i];
					minIndex = i;
				}
			}
		}

		if(minIndex == -1) {
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

	int completionTimes[5] = {0};
	float avgSjfWaitTime = 0.0;

	for(int i = 0; i < lineCount; i++) {
		for(int j = 0; j < currentTime; j++) {
			if(sjfSchedule[j] == i) {
				completionTimes[i] = j + 1;
			}
		}
		avgSjfWaitTime += (float)completionTimes[i] - (float)arrivalTimes[i] - (float)burstTimes[i];
	}
	avgSjfWaitTime /= (float)lineCount;

	int startExecTime[5] = {0};
	float avgSjfResponseTime = 0.0;
	for(int i = 0; i < lineCount; i++) {
		for(int j = 0; j < currentTime; j++) {
			if(sjfSchedule[j] == i) {
				startExecTime[i] = j;
				break;
			}
		}
		avgSjfResponseTime += (float)startExecTime[i] - (float)arrivalTimes[i];
	}
	avgSjfResponseTime /= (float)lineCount;

	int sjfThroughput = 0;
	for(int i = 0; i < lineCount; i++) {
		if(completionTimes[i] <= 10) {
			sjfThroughput++;
		}
	}

	printf("\tAverage Wait Time: %f\n", avgSjfWaitTime);
	printf("\tAverage Response Time %f\n", avgSjfResponseTime);
	printf("\tThroughput over 10 cycles: %d\n\n", sjfThroughput);

	//Round robin was inspired by the code I got for SJF. I used the same format and had
	//AI do some touch ups, but I mostly did it myself.
	printf("Round Robin\n\n");
	int quantum = 1;
	Queue p_queue;
	initQueue(&p_queue);
	int runningProcess = -1;
	int time = 0;
	int rrSchedule[totalTime + 100];
	int completedRR[5] = {0};
	int added[5] = {0};
	int numCompletedRR = 0;
	int remaining[5];

	for(int i = 0; i < lineCount; i++) {
		remaining[i] = burstTimes[i];
	}

	for(int i = 0; i < totalTime + 100; i++) {
		rrSchedule[i] = -1;
	}

	while(numCompletedRR < lineCount) {
		for(int i = 0; i < lineCount; i++) {
			if(!added[i] && time >= arrivalTimes[i]) {
				enqueue(&p_queue, i);
				added[i] = 1;

			}
		}

		if(isEmpty(&p_queue)) {
			rrSchedule[time] = -1;
			time++;
			continue;
		}

		runningProcess = dequeue(&p_queue);
		rrSchedule[time] = runningProcess;
		remaining[runningProcess] -= quantum;

		if(remaining[runningProcess] == 0) {
			completedRR[runningProcess] = 1;
			numCompletedRR++;
		}
		else {
			for(int i = 0; i < lineCount; i++) {
				if(!added[i] && arrivalTimes[i] <= time + 1) {
					enqueue(&p_queue, i);
					added[i] = 1;
				}
			}
			enqueue(&p_queue, runningProcess);
		}
		time++;
	}

	for(int i = 0; i < lineCount; i++) {
		printf("\t%s\t", names[i]);
		for(int j = 0; j < currentTime; j++) {
			if(rrSchedule[j] == i) {
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

	int completionTimesRR[5] = {0};
	float avgRRWaitTime = 0.0;

	for(int i = 0; i < lineCount; i++) {
		for(int j = 0; j < time; j++) {
			if(rrSchedule[j] == i) {
				completionTimesRR[i] = j + 1;
			}
		}
		avgRRWaitTime += (float)completionTimesRR[i] - (float)arrivalTimes[i] - (float)burstTimes[i];
	}
	avgRRWaitTime /= (float)lineCount;

	int startExecTimeRR[5] = {0};
	float avgRRResponseTime = 0.0;
	for(int i = 0; i < lineCount; i++) {
		for(int j = 0; j < time; j++) {
			if(rrSchedule[j] == i) {
				startExecTimeRR[i] = j;
				break;
			}
		}
		avgRRResponseTime += (float)startExecTimeRR[i] - (float)arrivalTimes[i];
	}
	avgRRResponseTime /= (float)lineCount;

	int rrThroughput = 0;
	for(int i = 0; i < lineCount; i++) {
		if(completionTimesRR[i] <= 10) {
			rrThroughput++;
		}
	}

	printf("\tAverage Wait Time: %f\n", avgRRWaitTime);
	printf("\tAverage Response Time %f\n", avgRRResponseTime);
	printf("\tThroughput over 10 cycles: %d\n\n", rrThroughput);

	fclose(fp);

	return 0;
}
