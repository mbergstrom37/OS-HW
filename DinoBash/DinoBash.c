#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

/*
	curses.h functions:
		initscr(); initializes the curses.h environment
		start_color(); initializes and enables terminal color support
			note default color is white on black background
		init_pair(int id, text_color, background_color) initializes a new color
		attron(COLOR_PAIR(int id)) turns on color with id id for the next text
		attroff(COLOR_PAIR(int id)) turns off color with id id
		echo() allows input text to be visible
		printw(char* str) is printf but curses.h
			it allows for format specifiers as well
		wgetnstr(stdscr, char *output, size_t size) is fgets
			same functionality, different parameter order
			fgets parameter order: fgets(char *output, size_t size, stdin)
		noecho() disables echo() done at the end to revert it back to original text state
		endwin() returns terminal to normal operating mode instead of curses.h mode
*/

//returns the message of the day
char *motd() {
	return "Welcome to Dino Bash!\n";
}

//main
int main() {
	//color initialization
	initscr();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);

	//print motd
	char *welcome = motd();
	attron(COLOR_PAIR(1));
	printw("%s", welcome);
	attroff(COLOR_PAIR(1));

	//variables needed
	int buffer_size = 100;
	int max_arg_amount = 2;
	char command_args[max_arg_amount][buffer_size];
	char command_str[buffer_size];

	//main loop
	while(1) {
		//empties command_args
		memset(command_args, 0, sizeof(command_args));

		//prints current working directory
		char *cwd = getcwd(NULL, 0);
		printw("%s$ ", cwd);

		//takes in input (echo() makes it visible on screen)
		echo();
		attron(COLOR_PAIR(2));
		wgetnstr(stdscr, command_str, buffer_size);
		attroff(COLOR_PAIR(2));

		//parse first word of command into commmand
		char *command = strtok(command_str, " \n");
		if(command == NULL) {
			continue;
		}
		//break main loop on exit
		if(strcmp(command, "exit") == 0) {
			break;
		}

		//parse the rest of command_str
		int arg_count = 0;
		for(int i = 0; i < max_arg_amount; i++) {
			char *curr_arg = strtok(NULL, " \n");
			if(curr_arg == NULL) {
				break;
			}
			arg_count++;
			strncpy(command_args[i], curr_arg, buffer_size);
		}

		//cd implementation
		if(strcmp(command, "cd") == 0) {
			//if user types just cd go to home directory
			if(command_args[0][0] == '\0') {
				char *home_dir = getenv("HOME");
				if(home_dir == NULL) {
					printw("Error: HOME environment variable is not set.\n");
				}
				else if(chdir(home_dir) != 0) {
					printw("Failed to change to home directory.\n");
				}
			}
			//if user types cd and an argument go to that directory (includes cd ..)
			else {
				if(chdir(command_args[0]) != 0) {
					printw("Directory %s cannot be found.\n", command_args[0]);
				}
			}

			//end this iteration
			continue;
		}

		//define the route for execlp()
		char route[100] = "/bin/";
		strncat(route, command, buffer_size - strlen(route) - 1);

		//start the fork
		pid_t pid;
		pid = fork();

		//fork error check
		if(pid < 0) {
			printw("Fork Failed.\nClosing DinoBash.");
			return 1;
		}
		//if in child process run execlp for the correct number of arguments
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
			refresh();
		}
		//if in parent process
		else {
			//background process check
			if((command_args[0][0] == '&' && command_args[0][1] == '\0') || (command_args[1][0] == '&' && command_args[1][1] == '\0')) {
				continue;
			}

			//program header, wait and program footer
			//note: it prints Starting program after the command output. it shouldn't.
			printw("----------------------------------Starting program---------------------------------\n");
			wait(NULL);
			printw("-----------------------------------Program Ended-----------------------------------\n");
		}
	}
	noecho();
	endwin();
	return 0;
}
