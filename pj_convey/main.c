#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "conway.h"

void clear_screen() {
	// Use ANSI escape codes to clear the screen
	printf("\e[1;1H\e[2J");
	printf("\e[?25l");
}

void print_help() {
	printf("\n--------------------------------------------------------\n");
	printf(
			"i<row><col>     initialize an empty grid with <row> rows and <col> "
			"columns\n");
	printf("r               randomly set alive/dead states for all grids\n");
	printf("n               evolve into next generation\n");
	printf(
			"c               automatically evolve,until receiving 'b' command pause "
			"evoution\n");
	printf("b               pause evolution\n");
	printf("s<path>         save grid states to file <path>\n");
	printf("l<path>         load grid states from file<path>\n");
	printf("q               quit\n");
	printf("h               help\n");
	printf("\n--------------------------------------------------------\n");
}

void print_game(const Conway* c) { show_conway(c); }

void* listen_break(void* flag) {
	char c = 0;
	while (c != 'b') {
		scanf(" %c", &c);
		scanf("%*[^\n]");	 // 清除未读内容
	}
	*(int*)flag = 1;
	return NULL;
}

void automatic_evolve(Conway* c) {
	int flag = 0;
	pthread_t listener;
	pthread_create(&listener, NULL, listen_break, &flag);
	while (flag != 1) {
		next_generation(c);
		print_game(c);
		printf("automatically evolving...\n");
		sleep(1);	 // 每秒演化一次
	}

	pthread_join(listener, NULL);
	print_game(c);
}

int main() {
	Conway c = new_conway(0, 0);
	int m = 0, n = 0;
	print_game(&c);
	print_help();
	while (1) {
		char cmd;
		char path[100];
		scanf(" %c", &cmd);
		switch (cmd) {
			case 'c':
				automatic_evolve(&c);
				break;
			case 'r':
				init_random(&c);
				print_game(&c);
				print_help();
				break;
			case 'i': {
				if (scanf("%d%d", &m, &n) != 2) {
					printf("THE INPUT IS NOT LEGAL!\n");
					print_help();
				}
				if (m >= 65535 || n >= 65535 || m <= 0 || n <= 0) {
					printf("THE INPUT IS NOT LEGAL!\n");
					print_help();
					break;
				}
				delete_grids(&c);
				c = new_conway(m, n);
				print_game(&c);
				print_help();
				break;
			}
			case 'n':
				next_generation(&c);
				print_game(&c);
				print_help();
				break;
			case 'b':
				pause();
				print_help();
				break;
			case 's':
				scanf(" %s", path);
				if (save_conway(&c, path) == 0) {
					printf("Grid saved successfully.\n");
				} else {
					printf("Failed to save grid.\n");
				}
				break;
			case 'l':
				scanf(" %s", path);
				delete_grids(&c);	 // Free existing memory
				c = new_conway_from_file(path);
				print_game(&c);
				break;
			case 'q':
				return 0;
			case 'h':
				print_help();
				break;
			// others
			default:
				printf("NOT DEFINED OPERATION!\n");
				break;
		}
	}

	return 0;
}