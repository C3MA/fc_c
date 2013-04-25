/*
 *  fcclient.c
 *  XCodeFC_C
 *
 *  Created by ollo on 24.04.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "fcclient.h"
#include "fc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>


#define BUFFERSIZE 2048

/*
 * creation of a new connection
 * allocating new memory.
 * YOU have to free this memory after using!
 */
extern fcclient_t* fcclient_new()
{
	fcclient_t* tmp = malloc(sizeof(fcclient_t));
	memset(tmp, 0, sizeof(fcclient_t));
	return tmp;
}

extern int fcclient_open(fcclient_t* fc, char* host)
{
	struct sockaddr_in serv_addr;
	
	uint8_t buffer[BUFFERSIZE];
    uint8_t output[BUFFERSIZE];
    int offset = 0;
	
	/* Open a connection to the wall */
	if (fc == NULL)
		return -1;
	
    if((fc->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return -2; /*FIXME return enum */
    }
    
    memset(&serv_addr, '0', sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NETWORK_PORT);
    
    if(inet_pton(AF_INET, host, &serv_addr.sin_addr)<=0)
    {
        /*FIXME ("\n inet_pton error occured\n"); */
        return -3;
    }
	
	if( connect(fc->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        /*FIXME ("\n Error : Connect Failed \n"); */
        return -4;
    }
	
	
	/* With the opened connection, ask the WALL for its resolution */
	    
    offset = send_inforequest(buffer, offset);
    printf("send, offset: %d \n",offset);
	
	add_header(buffer, output, offset);
	write(fc->sockfd, output, offset+HEADER_LENGTH);
		
	return 1; /* everything was done successful */
}


