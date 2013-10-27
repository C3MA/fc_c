/*
 *  hwal.c
 *
 * Hardware abstraction layer
 * ==========================
 *
 * Implementation for POSIX conform operating systems
 * 
 *  Created by ollo on 20.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "../hwal.h"
#include <arpa/inet.h>
#include <unistd.h>

#include <fcntl.h>

#define MAXFILEDESCRIPTORS	10

/* a small mapping table to abstract from the filedescriptors */
FILE*	fd_mappingtable[MAXFILEDESCRIPTORS];
int	fdNextFreecount=1; /* do not use the number zero, as zero marks errors while opening */

extern int hwal_fopen(char *filename, char* type)
{
	int usedMapIndex = fdNextFreecount;
	if (usedMapIndex > MAXFILEDESCRIPTORS)
	{
		return 0; /* reached the maximum possible files */
	}

	/* secure, that each map entry is only used once: */
	fdNextFreecount++;
	
	/* human counting adapted to the array (therefore the -1) */
	fd_mappingtable[usedMapIndex - 1] = fopen(filename, type);
	
	/* check if everything worked */
	if ( fd_mappingtable[usedMapIndex -1] == NULL)
	{
		return 0; /* damn it did not work */
	}

	return usedMapIndex;
}
	
extern int hwal_fread(void* buffer, int length, int filedescriptor)
{
	return fread(buffer, 1, length, fd_mappingtable[filedescriptor -1]);
}
	
	
extern void hwal_fclose(int filedescriptor)
{
	fclose(fd_mappingtable[filedescriptor -1]);
	/* FIXME free the entries in the mapping table */
}

extern void hwal_memset(void *buffer, int item, int length)
{
	memset(buffer, item, length);
}

extern void hwal_memcpy(void *s1, void* s2, int length)
{
	memcpy(s1, s2, length);

}

extern int hwal_strlen(char* text)
{
	return strlen(text);
}

extern void* hwal_malloc(int size)
{
	return malloc(size);
}

extern void hwal_free(void* memory)
{
	return free(memory);
}

extern void hwal_debug(char* codefile, int linenumber, char* text, ...)
{
	va_list ap;
	printf("%s:%d ", codefile, linenumber);
	va_start(ap, text);
	vprintf(text, ap);
	va_end(ap);	
	printf("\n");
}


extern int hwal_socket_tcp_new(int port, int maximumClients)
{
	int sockfd = 0;
	struct sockaddr_in srcAddress;
	int flags;
	
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        DEBUG_PLINE("Error : Could not create socket");
        return -1;
    }
	
	memset((char *)&srcAddress, 0, sizeof(srcAddress));
    srcAddress.sin_family       = AF_INET;
    srcAddress.sin_addr.s_addr  = htonl(0);
    srcAddress.sin_port         = htons(port);
	
    if(-1 == bind(sockfd,(struct sockaddr *)&srcAddress, sizeof(srcAddress)))
    {
		DEBUG_PLINE("error bind failed");
		return -2;
    }
	
    if(-1 == listen(sockfd, maximumClients))
    {
		DEBUG_PLINE("error listen failed");
		return -3;
    }
	
	
	/*Make connection noneblocking ( needed for the accept) */
	if (-1 == (flags = fcntl(sockfd, F_GETFL, 0)))
	{
		DEBUG_PLINE("Could not switch to nonblocking");
		return -4;
	}
	else
	{
		fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
		
	}

	
	return sockfd;
}

extern int hwal_socket_tcp_accet(int socketfd)
{
	struct sockaddr_in  clientAddr;
	socklen_t           sockLen = sizeof(clientAddr);
	int client;
		
	client = accept(socketfd, (struct sockaddr *) &clientAddr, &sockLen);
	if (client > 0)
	{
		DEBUG_PLINE("New Client connected %s:%d [Socket %d]", 
					inet_ntoa(clientAddr.sin_addr), 
					clientAddr.sin_port,
					client);
	}
	return client;
}

extern int hwal_socket_tcp_read(int clientSocket, uint8_t* workingMem, uint32_t size)
{
	/* some variables about the connected client */
	struct sockaddr_in  srcAddr;
	socklen_t           sockLen = sizeof(srcAddr);
	
	/* read something */
	return recvfrom(clientSocket,
				 (void *) workingMem,
				 size,
				 0,
				 (struct sockaddr *) &srcAddr,
				 &sockLen);
}

extern int hwal_socket_tcp_write(int clientSocket, uint8_t* data, uint32_t size)
{
	return write(clientSocket, data, size);
}
