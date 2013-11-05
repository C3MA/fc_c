/*
 *  serverMain.c
 *
 *  Created by ollo on 16.10.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

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

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
	printf("Caught signal %d\n",signum);
	
	
	printf("Stopping the Server...\n");
	
	/* clean everything */
	fcserver_close(&server);
	
	// Terminate program
	exit(signum);
}

/******************************************************************************
 * ENTRY POINT
 ******************************************************************************/
int main(int argc, char *argv[])
{
	fcserver_ret_t	ret;
	
	ret = fcserver_init(&server, &onNewImage, &onClientChange, 10, 12);
	if (ret != FCSERVER_RET_OK)
	{
		printf("Server initialization failed with returncode %d\n", ret);
		return 1;
	}
	
	fcserver_setactive(&server, 1 /* TRUE */);
	
	do {
		ret = fcserver_process(&server);
		
		usleep(10000); /* wait 10ms */
	} while ( ret == FCSERVER_RET_OK);
	
	/* clean everything */
	fcserver_close(&server);
	
    return 0;
}