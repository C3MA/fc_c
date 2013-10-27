/*
 *  serverMain.c
 *
 *  Created by ollo on 16.10.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include "fcserver.h"

/******************************************************************************
 * IMPLEMENTATION FOR THE NECESSARY CALLBACKS
 ******************************************************************************/

void onNewImage(uint8_t* rgb24Buffer, int width, int height)
{
	int i;
		
	//printf("%d x %d\n", width, height);
	for (i=1; i <= width * height; i++) {
		printf("%2X %2X %2X |", rgb24Buffer[i * 3 + 0], 
			   rgb24Buffer[i * 3 + 1], rgb24Buffer[i * 3 + 2]);
		
		if (i % width == 0)
		{
			printf("\n");
		}
	}
	printf("\n------------------------------------------\n");
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
	
	ret = fcserver_init(&server, &onNewImage, 10, 12);
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