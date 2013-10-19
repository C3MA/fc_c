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
    if(-1 == bind(sockfd,(struct sockaddr *)&srcAddress, sizeof(srcAddress)))
    {
		perror("error bind failed");
		close(sockfd);
		exit(EXIT_FAILURE);
    }
	
    if(-1 == listen(sockfd, 10 /* maximum paralell connections */))
    {
		perror("error listen failed");
		close(sockfd);
		exit(EXIT_FAILURE);
    }
	
	int clientSocket = accept(sockfd, NULL, NULL);
	
	if(0 > clientSocket)
	{
        perror("error accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
	}
	
	/* perform read write operations ... */
	/* n = read(clientSocket, recvBuff, size);*/
	printf("New client has connected");
	
    do {
		n = recvfrom(clientSocket,
                           (void *) recvBuff,
                           size,
                           0,
                           (struct sockaddr *) &srcAddr,
                           &sockLen);
		printf("Loop %ld bytes / max of %d\n", n, (int) size);
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
	
	/* Close all: client and server socket */
	close(clientSocket);
	close(sockfd);
}
