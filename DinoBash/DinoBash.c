#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	printf("Welcome to Dino Bash\n");
	int buffer_size = 100;
	int max_arg_amount = 2;
	char command_args[max_arg_amount][buffer_size];
	char command_str[buffer_size];
	while(1) {
		char *cwd = getcwd(NULL, 0);
		printf("%s$ ", cwd);
		fgets(command_str, buffer_size, stdin);
		char *command = strtok(command_str, " \n");
		if(strcmp(command, "exit") == 0) {
			break;
		}

		int arg_count = 0;
		for(int i = 0; i < max_arg_amount; i++) {
			char *curr_arg = strtok(NULL, " \n");
			if(curr_arg == NULL) {
				break;
			}
			arg_count++;
			strncpy(command_args[i], curr_arg, buffer_size);
		}

		char route[100] = "/bin/";
		strncat(route, command, buffer_size - strlen(route) - 1);

		if(strcmp(command, "cd") == 0) {
			if(command_args[0][0] == '\0') {
				char *home_dir = getenv("HOME");
				if(home_dir == NULL) {
					fprintf(stderr, "Error: HOME environment variable is not set.\n");
				}
				else if(chdir(home_dir) != 0) {
					fprintf(stderr, "Failed to change to home directory.\n");
				}
			}
			else {
				if(chdir(command_args[0]) != 0) {
					fprintf(stderr, "Directory %s cannot be found.\n", command_args[0]);
				}
			}

			continue;
		}
		pid_t pid;
		pid = fork();
		if(pid < 0) {
			fprintf(stderr, "Fork Failed.\nClosing DinoBash.");
			return 1;
		}
		else if(pid == 0) {
			if(arg_count == 0) {
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
	}

	return 0;
}
