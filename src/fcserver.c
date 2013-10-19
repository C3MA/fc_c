/*
 *  fcserver.c
 *
 *  Created by ollo on 16.10.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "fcseq.h"
#include "fc.h"
#include "hwal.h"
#include "fcserver.h"

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/


fcserver_ret_t fcserver_init (fcserver_t* server, ImageCallback_t onNewImage, int width, int height)
{
	DEBUG_PLINE("Server-Init");
	/* Clean the memory structure (as a new server is started) */
	hwal_memset(server, 0, sizeof(fcserver_t));
	
	/* Open a new server TCP connection */
	server->serversocket = hwal_socket_tcp_new(NETWORK_PORT, FCSERVER_MAXCLIENT);
	if (server->serversocket <= 0)
	{
		return FCSERVER_RET_IOERR;
	}
	
	/* store the callbacks */
	server->onNewImage = onNewImage;
	server->tmpMemSize = FCSERVER_TEMPMEM_MAX;
	server->tmpMem = hwal_malloc(server->tmpMemSize);
	server->width = width;
	server->height = height;
	
	return FCSERVER_RET_OK;
}

/** @fn fcserver_ret_t store_client_in (fcserver_t* server, int clientSocket)
 * @brief Stores a new client in the internal structure
 * This function will search for free space in the @see fcserver_t->clientsocket buffer
 *
 * @param[in,out]	server			structure, where the client should be put in
 * @param[in]		clientSocket	that, is new and has to be remembered
 * @return status 
 */
fcserver_ret_t store_client_in (fcserver_t* server, int clientSocket)
{
	int i;
	if (server == NULL || clientSocket <= 0)
	{
		return FCSERVER_RET_PARAMERR;
	}
	
	/* Search a free spot */
	for (i=0; i < FCSERVER_MAXCLIENT; i++) {
		if (server->clientsocket[i] == 0)
		{
			server->clientsocket[i] = clientSocket;
			return FCSERVER_RET_OK;
		}
	}
	
	/* We have not enough space for this new interested one */
	return FCSERVER_RET_OUTOFMEMORY;
}

/** @fn fcserver_ret_t fcserver_process (fcserver_t* server, int clientSocket)
 * @brief Speak with the client
 *
 * @param[in]	server			structure, where the client should be put in
 * @param[in]	clientSocket	to talk to.
 * @return status 
 */
static fcserver_ret_t process_client (fcserver_t* server, int clientSocket)
{
	int n, offset;
    int type=-1;
    int length = 0;
	
	n = hwal_socket_tcp_read(clientSocket, server->tmpMem, server->tmpMemSize);
	/* next try if nothing was received */
	if (n == -1)
	{
		return FCSERVER_RET_NODATA;
	}
	
	/* First check, if the Client has something to say */
	if (n < HEADER_LENGTH)
	{
		DEBUG_PLINE("Error : Network read error");
		return FCSERVER_RET_IOERR;
	}
	offset = get_header(server->tmpMem, 0, &type, &length);
	if (offset == -1)
	{
		DEBUG_PLINE("Error : Could not analyze header");
		return FCSERVER_RET_IOERR;
	}
	
	DEBUG_PLINE("New Header typ: %d length of information: %d\n",type,length);
	
	/* Decode this information */
	switch (type)
	{
		case SNIPTYPE_PING:
		case SNIPTYPE_PONG:
		case SNIPTYPE_ERROR:
		case SNIPTYPE_REQUEST:
		case SNIPTYPE_START:
		case SNIPTYPE_FRAME:
		case SNIPTYPE_ACK:
		case SNIPTYPE_NACK:
		case SNIPTYPE_TIMEOUT:
		case SNIPTYPE_ABORT:
		case SNIPTYPE_EOS:
		case SNIPTYPE_INFOANSWER:
		default:
			DEBUG_PLINE("%d is not implemented",type);
			break;
		case SNIPTYPE_INFOREQUEST:
		{
			uint8_t *output = hwal_malloc(1280); hwal_memset(output, 0, 1280);
			uint8_t *buffer = hwal_malloc(1024); hwal_memset(output, 0, 1024);
			uint8_t *meta	= hwal_malloc(1024); hwal_memset(output, 0, 1024);			
			int offset_meta = create_metadata(meta, 0, FCSERVER_DEFAULT_FPS, 
										  server->width, server->height, 
										  FCSERVER_DEFAULT_NAME,
										  FCSERVER_DEFAULT_VERSION);
			offset = send_infoanswer(buffer, offset, meta, offset_meta);
			add_header(buffer, output, offset);
			hwal_socket_tcp_write(clientSocket, output, offset+HEADER_LENGTH);
			
			hwal_free(meta);
			hwal_free(buffer);
			hwal_free(output);
		}
			break;
	}
	
	
	return FCSERVER_RET_NOTIMPL;
}

fcserver_ret_t fcserver_process (fcserver_t* server)
{
	int client;
	int i;
	
	/* Check for new waiting clients */
	if (server == 0 || server->serversocket <= 0)
	{
		return FCSERVER_RET_PARAMERR;
	}
	
	/** handle all actual connected clients **/
	/* search for new waiting ones */
	client = hwal_socket_tcp_accet(server->serversocket);
	
	if (client > 0)
	{
		if (server->clientcount < (FCSERVER_MAXCLIENT - 1))
		{
			server->clientcount++;
			store_client_in(server, client);
		}
		else
		{
			DEBUG_PLINE("No Space for new client");
			/* FIXME tell the client "NOOOOO, go a way!" */
		}
	}
	
	/* handle all open connections */
	for (i=0; i < FCSERVER_MAXCLIENT; i++)
	{
		if (server->clientsocket[i] > 0)
		{
			/* Found an open client ... speak with him */
			process_client(server, server->clientsocket[i]);
		}
	}
	
	
	return FCSERVER_RET_OK;
}

fcserver_ret_t fcserver_close (fcserver_t* server)
{
	if (server == NULL)
		return FCSERVER_RET_PARAMERR;
		
	
	if (server->tmpMem > 0)
	{
		hwal_free(server->tmpMem);
	}
	
	return FCSERVER_RET_OK;
}
