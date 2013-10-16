/*
 *  serverMain.c
 *
 *  Created by ollo on 16.10.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <stdio.h>
#include "fcserver.h"

/******************************************************************************
 * IMPLEMENTATION FOR THE NECESSARY CALLBACKS
 ******************************************************************************/

void onNewImage(uint8_t* rgb24Buffer, int width, int height)
{
	
}

void onNewClient(void)
{
	
}

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 * ENTRY POINT
 ******************************************************************************/
int main(int argc, char *argv[])
{
	fcserver_ret_t	ret;
	fcserver_t		server;
	
	ret = fcserver_init(&server, &onNewImage, &onNewClient);
	printf("Server initialization returned %d\n", ret);
    return 0;
}