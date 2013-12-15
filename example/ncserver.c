/* @file ncserver.c
 * @brief Server simulation with an output via ncurses
 * @author ollo
 *
 * @date 15.12.2013 – Created this module
 * @defgroup ncurses_server
 * 
 * Copyright 2013 C3MA. All rights reserved.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <curses.h>

#include "fcserver.h"

/******************************************************************************
 * IMPLEMENTATION FOR THE NECESSARY CALLBACKS
 ******************************************************************************/

void onNewImage(uint8_t* rgb24Buffer, int width, int height)
{
	int i;

	//printf("%d x %d\n", width, height);
	for (i=0; i < width * height; i++) {
		printf("%02X%02X%02X|", rgb24Buffer[i * 3 + 0],
			   rgb24Buffer[i * 3 + 1], rgb24Buffer[i * 3 + 2]);

		if ( (i + 1) % width == 0)
		{
			printf("\n");
		}
	}
	printf("------------------------------------------\n");
}

void onClientChange(uint8_t totalAmount, fclientstatus_t action, int clientsocket)
{
	printf("Callback client %d did %X\t[%d clients]\n", clientsocket, action, totalAmount);
}

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

static fcserver_t		server;

void quit()
{
	/* clean everything */
	fcserver_close(&server);

	endwin();
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
	printf("Caught signal %d\n",signum);


	printf("Stopping the Server...\n");

	quit();

	// Terminate program
	exit(signum);
}

void printUsage(char *appl)
{
	printf("Usage: %s <width> <height>\n", appl);
}

int main(int argc, char *argv[])
{
	int width;
	int height;
	fcserver_ret_t	ret;
	int sleeptime;

	/* ncurses variables */
	int x, y;

	if (argc < 3)
	{
		printUsage(argv[0]);
		return 2;
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);


	ret = fcserver_init(&server, &onNewImage, &onClientChange, width, height);
	if (ret != FCSERVER_RET_OK)
	{
		printf("Server initialization failed with returncode %d\n", ret);
		return 1;
	}

	fcserver_setactive(&server, 1 /* TRUE */);

	/* initialize the ncurses environment */
	initscr();
	atexit(quit);
	start_color();
	clear();

	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	color_set(1, 0);
	bkgd(COLOR_PAIR(1)); /* Fill the complete background */

	mvprintw(3, 5, "LINES: %d", LINES);
	mvprintw(4, 5, "COLS: %d", COLS);
	getyx(stdscr, y, x);
	mvprintw(5, 5, "Current Cursor position: [%d, %d]", y, x);
	getbegyx(stdscr, y, x);
	mvprintw(6, 5, "origin: [%d, %d]", y, x);
	getmaxyx(stdscr, y, x);
	mvprintw(7, 5, "Window dimension: [%d, %d]", y, x);
	refresh();


	sleeptime = 10;
	do {
		ret = fcserver_process(&server, sleeptime);

		usleep( sleeptime * 1000 );/* wait 10ms */
	} while ( ret == FCSERVER_RET_OK);

	/* clean everything */
	fcserver_close(&server);

	return 0;
}
