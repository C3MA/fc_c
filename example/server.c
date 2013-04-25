/*
 *  server.c
 *  XCodeFC_C
 *
 *  Created by ollo on 25.04.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "fc.h"

/*
 * Small implementation of a server using the test module for the logic
 */
int main(int argc, char *argv[])
{
	printf("Starting a server\n");
	
	
    int sockfd = 0;
    long n = 0;
	
	size_t size = 2048;
    char recvBuff[size];
    uint8_t buffer[2048];
    uint8_t output[2048];
    int offset;
    int type=-1;
    int length = 0;
    
	struct sockaddr_in srcAddress;
	
	struct sockaddr_in  srcAddr;
	socklen_t           sockLen = sizeof(srcAddr);
	
    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
	
	memset((char *)&srcAddress, 0, sizeof(srcAddress));
    srcAddress.sin_family       = AF_INET;
    srcAddress.sin_addr.s_addr  = htonl(0);
    srcAddress.sin_port         = htons(NETWORK_PORT);
    n = accept(sockfd, (struct sockaddr *) &srcAddress, &sockLen);
	if (n < 0 )
	{
		printf("Could not listen for network connections [%ld]\n", n);
		return 2;
	}
	
    do {
		n = recvfrom(sockfd,
                           (void *) recvBuff,
                           size,
                           0,
                           (struct sockaddr *) &srcAddr,
                           &sockLen);
		/* next try if nothing was received */
		if (n == -1)
			continue;
		
        if (n < HEADER_LENGTH) {
            printf("\n Error : Network read error\n");
            return 1;
        }
        offset = get_header((uint8_t*)recvBuff, 0, &type, &length);
        if (offset == -1) {
            printf("\n Error : Could not analyze header\n");
            return 1;
        }
        printf("typ: %d laenge: %d\n",type,length);
    } while (type == -1);
	
}
