#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	printf("Welcome to Dino Bash\n");
	int buffer_size = 100;
	int max_arg_amount = 2;
	char command_args[max_arg_amount][buffer_size];
	char *command_ptr = malloc(buffer_size * sizeof(char));
	char command_str[buffer_size];
	if(command_ptr == NULL) {
		fprintf(stderr, "Malloc Failed\n");
		return 1;
	}
	int space_count = 0;
	for(int i = 0; i < buffer_size; i++) {
		if(command_str[i] == ' ') {
			space_count++;
		}
	}
	while(1) {
		char *cwd = getcwd(NULL, 0);
		printf("%s$ ", cwd);
		fgets(command_ptr, buffer_size, stdin);
		strncpy(command_str, command_ptr, buffer_size);
		char *command;
		if(space_count == 0) {
			command = strtok(command_str, "\0");
		}
		else {
			char *command = strtok(command_str, " ");
			space_count--;
		}
		if(command == "exit") {
			break;
		}

		int arg_count = 0;
		for(int i = 0; i < max_arg_amount; i++) {
			char *curr_arg;
			if(space_count == 0) {
				curr_arg = strtok(NULL, "\0");
			}
			else {
				curr_arg = strtok(NULL, " ");
				space_count--;
			}
			if(curr_arg == NULL) {
				break;
			}
			arg_count++;
			strncpy(command_args[i], curr_arg, buffer_size);
		}

		char route[buffer_size] = "/bin/";
		strncat(route, command, buffer_size - strlen(route) - 1);
		pid_t pid;
		pid = fork();
		if(pid < 0) {
			fprintf(stderr, "Fork Failed.\nClosing DinoBash.");
			return 1;
		}
		else if(pid == 0) {
			if(command == "cd") {
				if(command_args[0][0] == '\0') {
					char *home_dir = getenv("HOME");
					if(home_dir == NULL) {
						fprintf(stderr, "Error: HOME environment variable is not set.\n");
					}
					if(chdir(home_dir) != 0) {
						fprintf(stderr, "Failed to change to home directory.\n");
					}
				}
				else {
					if(chdir(command_args[0]) != 0) {
						fprintf(stderr, "No %s directory exists\n", command_args[0]);
					}
				}
			}
			else if(arg_count == 0) {
				execlp(route,command,NULL);
			}
			else if(arg_count == 1) {
				execlp(route,command,command_args[0],NULL);
			}
			else if(arg_count == 2) {
				execlp(route,command,command_args[0],command_args[1],NULL);
			}
		}
		else {
			wait(NULL);
			printf("Child Complete\n");
		}
		command_ptr[0] = '\0';
	}
	free(command_ptr);
	command_ptr = NULL;
}
