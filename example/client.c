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
	
	uint8_t frame[client->width * client->height * 20]; 
	int x, y;
	x = 0;
	y = 0;
	
	/* master file descriptor list */
	fd_set rfds;
	
	/* clear the master and temp sets */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	
	struct timeval tv;
	/* Wait up to five seconds. */
    tv.tv_sec = 0;
    tv.tv_usec = 0;
	
	/* add the listener to the master set */
	FD_SET(client->sockfd, &rfds);
	printf("============= Sending Frame =============\n");
	while (1) {
		/* call this function until we were successfull in receiving something */
		
		if( select(1, &rfds, NULL, NULL, &tv) != -1 )	
		{
			success = fcclient_processNetwork(client);
			printf("Received data [%d]\n", success);
		}
		
		if (client->connected) {
			for (int x1=0; x1 < client->width; x1++) {
				for (int y1=0; y1 < client->height; y1++) {
					if (x1 == x && y1 == y)
					{
						fcclient_addPixel(client, frame, 0, 0, 255, x1, y1);
					}
					else
					{
						fcclient_addPixel(client, frame, 0, 0, 0, x1, y1);
					}

				}
			}
			
			/* Now we need to send some nice frames to the wall */
			fcclient_sendFrame(client, frame);
			x += 1;
			y += 1;
		}
		
		if (x > client->width && y > client->height)
		{
			x = 0;
			y = 0;
		}
		
		/*FIXME update the function using "select()" with an timeout */
//		usleep(20);
	}
		
	
	printf("Reached the end of the client, good bye\n");
	return 0;
}