#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

char *motd() {
	return "Welcome to Dino Bash!\n";
}

int main()
{
	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);

	char *welcome = motd();
	attron(COLOR_PAIR(1));
	printw("%s", welcome);
	attroff(COLOR_PAIR(1));

	int buffer_size = 100;
	int max_arg_amount = 2;
	char command_args[max_arg_amount][buffer_size];
	char command_str[buffer_size];
	while(1) {
		memset(command_args, 0, sizeof(command_args));
		char *cwd = getcwd(NULL, 0);
		printw("%s$ ", cwd);
		echo();
		attron(COLOR_PAIR(2));
		wgetnstr(stdscr, command_str, buffer_size);
		attroff(COLOR_PAIR(2));
		char *command = strtok(command_str, " \n");
		if(command == NULL) {
			continue;
		}
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
					printw("Error: HOME environment variable is not set.\n");
				}
				else if(chdir(home_dir) != 0) {
					printw("Failed to change to home directory.\n");
				}
			}
			else {
				if(chdir(command_args[0]) != 0) {
					printw("Directory %s cannot be found.\n", command_args[0]);
				}
			}

			continue;
		}
		pid_t pid;
		pid = fork();
		if(pid < 0) {
			printw("Fork Failed.\nClosing DinoBash.");
			return 1;
		}
		else if(pid == 0) {
			endwin();
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
			move(getcury(stdscr) + 1, 0);
			printw("----------------------------------Starting program---------------------------------\n");
			wait(NULL);
			printw("-----------------------------------Program Ended-----------------------------------\n");
		}
	}
	noecho();
	endwin();
	return 0;
}
