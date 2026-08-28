#include "LuckyNumber.h"

int checkLuckyNumber(int guess) {
	srand(time(NULL));

	int random = (rand() % 5) + 1;
	return guess-random;
}
