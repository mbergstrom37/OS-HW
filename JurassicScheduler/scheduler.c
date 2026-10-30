#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
	FILE *fp = fopen("text.txt", "r");
	char ch[100];
	char lines[5][100];
	char names[5][100];
	int times[5][2];

	if(fp == NULL) {
		printf("Unable to open file.\n");
		return 1;
	}
	int i = 0;
	while(fgets(ch, sizeof(ch), fp)) {
		strncpy(lines[i], ch, sizeof(lines[i]) - 1);
		lines[i][sizeof(lines[i]) - 1] = '\0';
		i++;
	}
	char *token;

	for(int i = 0; i < 5; i++) {
		token = strtok(lines[i], "\t\n");
		strncpy(names[i], token, sizeof(names[i]) - 1);
		names[i][sizeof(names[i]) - 1] = '\0';
		printf("%s\t", names[i]);
		int j = 0;
		while(token != NULL) {
			token = strtok(NULL, "\t\n");
			if(j < 2) {
				times[i][j] = atoi(token);
				printf("%d\t",times[i][j]);
				j++;
			}
		}
		printf("\n");
	}

	fclose(fp);

	return 0;
}
