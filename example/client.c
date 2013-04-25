/*
 *  client.c
 *  XCodeFC_C
 *
 *  Created by ollo on 24.04.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include "fcclient.h"

int main(int argc, char *argv[])
{
	int success;
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
		/*FIXME update the function using "select()" with an timeout */
		
	} while (success);
	
	printf("Reached the end of the client, good bye\n");
	return 0;
}