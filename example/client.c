/*
 *  client.c
 *  XCodeFC_C
 *
 *  Created by ollo on 24.04.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */
#include <stdio.h>
#include <unistd.h>

#include "fcclient.h"

int main(int argc, char *argv[])
{
	int success, success2;
	fcclient_t * client = fcclient_new();
	
	if (argc < 2)
	{
		printf("Usage %s <IP address of the wall>\n", argv[0]);
		return 1;
	}
	
	printf("Connecting to %s...\n", argv[1]);
	success = fcclient_open(client, argv[1]);
	printf("Connection returned %d\n", success);
	
	if (!success) {
		printf("Cannot connect to wall\n");
		return 1;
	}
	
	do {
		/* call this function until we were successfull in receiving something */
		success = fcclient_processNetwork(client);
		printf("Network: %d\n", success);
		/*FIXME update the function using "select()" with an timeout */
	} while (!success);
	/* the server has answered */
	
	success2 = fcclient_start(client);
	printf("Start: %d\n", success2);
	
	uint8_t frame[client->width * client->height * 10]; 
	int x, y;
	x = 0;
	y = 0;
	
	while (1) {
		/* call this function until we were successfull in receiving something */
		success = fcclient_processNetwork(client);
		if (client->connected) {
			printf("Connected :-)\n");
			
			fcclient_addPixel(client, frame, 0, 0, 255, x, y);
			
			/* Now we need to send some nice frames to the wall */
			fcclient_sendFrame(client, frame);
			sleep(1);
			x += 1;
			y += 1;
		}
		/*FIXME update the function using "select()" with an timeout */
	}
		
	
	printf("Reached the end of the client, good bye\n");
	return 0;
}