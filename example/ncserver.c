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

#include <panel.h>
#include <curses.h>

#include "fcserver.h"

#define START_X_OFFSET 1
#define START_Y_OFFSET 1
#define COLOR_OFFSET 	10 /**< The first 10 colors can be used in the application, the upper ones are used to visualaize the wall */

/******************************************************************************
 * Local variable
 ******************************************************************************/

static PANEL *pnlSimulation;
static WINDOW *winSimulation;

static fcserver_t		server;

/******************************************************************************
 * IMPLEMENTATION FOR THE NECESSARY CALLBACKS
 ******************************************************************************/

void onNewImage(uint8_t* rgb24Buffer, int width, int height)
{
	int i;
	int ncX = START_X_OFFSET;
	int ncY = START_Y_OFFSET;

	color_set(1, 0);
	mvprintw(0, 0, "Frame dimension: %2d x %2d", width, height);

	/* Display the current wall on the virtual ncruses */

	/*
	for (i=0; i < width * height; i++)
	{
		init_color(COLOR_OFFSET + i, rgb24Buffer[i * 3 + 0],
				   rgb24Buffer[i * 3 + 1], rgb24Buffer[i * 3 + 2]);
		mvaddstr(ncY, ncX, "X");
		ncX++;

		if ( (i + 1) % width == 0)
		{
			ncY++;
			ncX=START_X_OFFSET;
		}
	} */

}

void onClientChange(uint8_t totalAmount, fclientstatus_t action, int clientsocket)
{
	mvprintw(LINES - 1, 1, "Callback client %d did %X [%d clients]", clientsocket, action, totalAmount);
	refresh();
}

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/

void quit()
{
	/* clean everything */
	fcserver_close(&server);

	del_panel(pnlSimulation);
	delwin(winSimulation);
	endwin();
}

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
	/* Caught signal */


	/* Stopping the Server... */
	quit();

	/* Terminate program */
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

	winSimulation = newwin(height, width, 1, 1);
	box(winSimulation, ACS_VLINE, ACS_HLINE);
	pnlSimulation = new_panel(winSimulation);

	update_panels();
	doupdate();

	mvprintw(LINES - 1, COLS - 30, "Server dimension [%2d, %2d]", width, height);
	refresh();


	sleeptime = 20;
	do {
		ret = fcserver_process(&server, sleeptime);

		usleep( sleeptime * 1000 );/* wait 10ms */
	} while ( ret == FCSERVER_RET_OK);

	/* clean everything */
	fcserver_close(&server);

	return 0;
}
