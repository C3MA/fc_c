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

#define BUFFERSIZE_OUTPUT			1280
#define BUFFERSIZE_SENDINGBUFFER	1024

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/


fcserver_ret_t fcserver_init (fcserver_t* server, ImageCallback_t onNewImage, ClientCallback_t	onClientChange,
							  int width, int height)
{
	DEBUG_PLINE("Server-Init");
	
	if (width <= 0 || height <= 0)
	{
		return FCSERVER_RET_PARAMERR;
	}
	
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
	server->onClientChange = onClientChange;
	
	server->tmpMemSize = FCSERVER_TEMPMEM_MAX;
	server->tmpMem = hwal_malloc(server->tmpMemSize);
	server->width = width;
	server->height = height;
	
	server->imageBuffer = hwal_malloc(width * height * 3);
	
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
		if (server->client[i].clientsocket == 0)
		{
			server->client[i].clientsocket = clientSocket;
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
 * @param[in]	client			the connected client to talk to.
 * @return status 
 */
static fcserver_ret_t process_client(fcserver_t* server, fcclient_t* client)
{
	int n, offset = 0;
    int type=-1;
    int length = 0;
	int write_offset = 0;
	
	/* FIXME the server->tmpMem should probalby exists for each client
	 * (even if only one client, connected to the wall can generate the huge packets) */
	n = hwal_socket_tcp_read(client->clientsocket, 
							 (server->tmpMem + server->reading_offset), 
							 (server->tmpMemSize - server->reading_offset));
		
	/*FIXME try to check if client is still connected FCSERVER_RET_CLOSED */
		
	/* First check, if the Client has something to say */
	if (n == -1)
	{
		/* no new packet found on the network */
		return FCSERVER_RET_NOTHINGNEW;
	}
	else if (n == 0)
	{
		return FCSERVER_RET_CLOSED;
	}
	else if (n < HEADER_LENGTH)
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
	
	/* Add the already extracted bytes to the new ones */
	n += server->reading_offset;
	
	DEBUG_PLINE("New Header typ: %d length of information: %d [fetched is %d byte from the network]",type,length, n);
	
	if (length > n)
	{
		server->reading_offset = n;
		DEBUG_PLINE("Update offset to %d", server->reading_offset);
	}
	else
	{
		/* reset the fragment detection for packets */
		server->reading_offset = 0;
		
		/* Decode the information */
		switch (type)
		{
			case SNIPTYPE_REQUEST:
			{
				char *color;
				int seqId;
				int meta_offset;
				int meta_length;
				int frames_per_second, width, heigth;
				char *generator_name;
				char *generator_version;
				
				offset = recv_request(server->tmpMem, offset, &color, &seqId, &meta_offset, &meta_length);
				if (offset == -1) {
					DEBUG_PLINE("recv_request Faild!");
				} else {
					DEBUG_PLINE("Parse Request, Color: %s, seqId: %d",color,seqId);
				}
				offset = parse_metadata(server->tmpMem,meta_offset,&frames_per_second, 
										&width, &heigth, &generator_name, &generator_version);
				if (offset == -1) {
					DEBUG_PLINE("parse Metadata Faild!");
					return -1;
				} else {
					DEBUG_PLINE("Metadata, fps: %d, width: %d, height: %d, gen._name: %s, gen._version: %s",
								frames_per_second,width,heigth,generator_name,generator_version);
				}
				
				/* allocate some memory for answering */
				uint8_t *output = hwal_malloc(BUFFERSIZE_OUTPUT); hwal_memset(output, 0, BUFFERSIZE_OUTPUT);
				uint8_t *buffer = hwal_malloc(BUFFERSIZE_SENDINGBUFFER); hwal_memset(output, 0, BUFFERSIZE_SENDINGBUFFER);
				
				/* Verify , if the client has the correct resolution */
				if (server->width == width && server->height == heigth)
				{
					client->clientstatus = FCCLIENT_STATUS_WAITING;
					/* Send the client an acknowledgement (ACK) */
					write_offset = send_ack(buffer, write_offset);
					DEBUG_PLINE("ACK Request send");
				}
				else
				{
					uint8_t buffer[BUFFERSIZE_SENDINGBUFFER];
					/* Inform the client with an error message */
					char descr[] = "Wrong Screen resolution";
					DEBUG_PLINE("Error while requesting: '%s'", descr);
					write_offset = send_error(buffer, write_offset, FCSERVER_ERR_RESOLUTION, descr);
				}
				
				/* send the corresponding message: Success or error */
				add_header(buffer, output, write_offset);
				hwal_socket_tcp_write(client->clientsocket, output, write_offset+HEADER_LENGTH);
				
				hwal_free(buffer);
				hwal_free(output);
				
				/* Free all resources needed for sending */
				hwal_free(color);
				hwal_free(generator_name);
				hwal_free(generator_version);
				break;
			}
			case SNIPTYPE_FRAME:
			{
				int x, y, red, green, blue;
				int frame_length;
				int frame_offset, frame_offset_start;
				int index;
				
				offset = recv_frame(server->tmpMem, offset, &frame_offset, &frame_length);
				if (offset == -1) {
					DEBUG_PLINE("recv_frame Faild!");
				} else {
					DEBUG_PLINE("Parse Frame, frame_length: %d",frame_length);
				}
				frame_offset_start = frame_offset;
				do {
					frame_offset = frame_parse_pixel(server->tmpMem,frame_offset, &red, &green, &blue, &x, &y);
					index = (((x * server->width) + y) * 3);
					server->imageBuffer[index + 0] = red;
					server->imageBuffer[index + 1] = green;
					server->imageBuffer[index + 2] = blue;				
				} while (frame_offset < (frame_offset_start+frame_length));
				
				if (server->onNewImage > 0)
				{
					server->onNewImage(server->imageBuffer, server->width, server->height);
				}
				break;
			}
			case SNIPTYPE_INFOREQUEST:
			{
				uint8_t *output = hwal_malloc(BUFFERSIZE_OUTPUT); hwal_memset(output, 0, BUFFERSIZE_OUTPUT);
				uint8_t *buffer = hwal_malloc(BUFFERSIZE_SENDINGBUFFER); hwal_memset(output, 0, BUFFERSIZE_SENDINGBUFFER);
				uint8_t *meta	= hwal_malloc(BUFFERSIZE_SENDINGBUFFER); hwal_memset(output, 0, BUFFERSIZE_SENDINGBUFFER);			
				int offset_meta = create_metadata(meta, 0, FCSERVER_DEFAULT_FPS, 
											  server->width, server->height, 
											  FCSERVER_DEFAULT_NAME,
											  FCSERVER_DEFAULT_VERSION);
				write_offset = send_infoanswer(buffer, write_offset, meta, offset_meta);
				add_header(buffer, output, write_offset);
				hwal_socket_tcp_write(client->clientsocket, output, write_offset+HEADER_LENGTH);
				DEBUG_PLINE("Answered %dx%d pixel (%d fps) for '%s' on version '%s'",server->width, server->height, FCSERVER_DEFAULT_FPS,
							FCSERVER_DEFAULT_NAME,
							FCSERVER_DEFAULT_VERSION);
				hwal_free(meta);
				hwal_free(buffer);
				hwal_free(output);
			}
				break;
			case SNIPTYPE_PING:
			case SNIPTYPE_PONG:
			case SNIPTYPE_ERROR:
			case SNIPTYPE_START:
			case SNIPTYPE_ACK:
			case SNIPTYPE_NACK:
			case SNIPTYPE_TIMEOUT:
			case SNIPTYPE_ABORT:
			case SNIPTYPE_EOS:
			case SNIPTYPE_INFOANSWER:
			default:
				DEBUG_PLINE("%d is not implemented",type);
				break;
		}
	}
	
	return FCSERVER_RET_OK;
}

fcserver_ret_t fcserver_process (fcserver_t* server)
{
	int client = 0;
	int i;
	int newClientStarting = 0;
	
	/* Check for new waiting clients */
	if (server == 0 || server->serversocket <= 0)
	{
		return FCSERVER_RET_PARAMERR;
	}
		
	/* Check if a new client can get the wall */
	for (i=0; i < FCSERVER_MAXCLIENT; i++)
	{
		/* search for already connected clients */
		if (server->client[i].clientstatus == FCCLIENT_STATUS_CONNECTED)
		{
			client = 1; /* reusing variable as flag to indicate an already connected client */
		}
		else if (newClientStarting == 0 && server->client[i].clientstatus == FCCLIENT_STATUS_WAITING)
		{
			/* Client %d is waiting", i */
			newClientStarting = (i + 1); /* count from 1 to FCSERVER_MAXCLIENT + 1 */
		}
	}
	
	if (!client)
	{
		/* no-one is actual using the wall, a new one can start now */
		if (newClientStarting) /* as the index starts at one we can detect here new clients */
		{
			int write_offset = 0;
			DEBUG_PLINE("Client %d has now the wall", server->client[newClientStarting - 1].clientsocket);
			server->client[newClientStarting - 1].clientstatus = FCCLIENT_STATUS_CONNECTED;
			
			if (server->onClientChange > 0)
			{
				server->onClientChange(server->clientcount, FCCLIENT_STATUS_CONNECTED, 
									   server->client[newClientStarting - 1].clientsocket);
			}
			
			/* allocate some memory for answering */
			uint8_t *output = hwal_malloc(BUFFERSIZE_OUTPUT); hwal_memset(output, 0, BUFFERSIZE_OUTPUT);
			uint8_t *buffer = hwal_malloc(BUFFERSIZE_SENDINGBUFFER); hwal_memset(output, 0, BUFFERSIZE_SENDINGBUFFER);
			
			write_offset = send_start(buffer, write_offset);
			
			/* send the corresponding message: Success or error */
			add_header(buffer, output, write_offset);
			hwal_socket_tcp_write(server->client[newClientStarting - 1].clientsocket, output, write_offset+HEADER_LENGTH);
			
			hwal_free(buffer);
			hwal_free(output);
		}
	}
	client = 0; /* Reset the temporary variable, for the original porpuse */
	
	/** handle all actual connected clients **/
	/* search for new waiting ones */
	client = hwal_socket_tcp_accet(server->serversocket);
	
	if (client > 0)
	{
		if (server->clientcount < (FCSERVER_MAXCLIENT - 1))
		{
			server->clientcount++;
			store_client_in(server, client);
			
			if (server->onClientChange > 0)
			{
				server->onClientChange(server->clientcount, FCCLIENT_STATUS_INITING, client);
			}
		}
		else
		{
			uint8_t buffer[BUFFERSIZE_SENDINGBUFFER];
			/* Inform the client with an error message */
			char descr[] = "No Space for new client";
			int write_offset = 0;
			
			uint8_t *output = hwal_malloc(BUFFERSIZE_OUTPUT); hwal_memset(output, 0, BUFFERSIZE_OUTPUT);
			
			write_offset = send_error(buffer, write_offset, FCSERVER_ERR_MAXCLIENTS, descr);			
			DEBUG_PLINE("No Space for new client");			
			/* send the corresponding message: Success or error */
			add_header(buffer, output, write_offset);
			hwal_socket_tcp_write(client, output, write_offset+HEADER_LENGTH);			
			hwal_socket_tcp_close(client);
			
			if (server->onClientChange > 0)
			{
				server->onClientChange(server->clientcount, FCCLIENT_STATUS_TOOMUTCH, client);
			}
		}
	}
	
	/* handle all open connections */
	for (i=0; i < FCSERVER_MAXCLIENT; i++)
	{
		if (server->client[i].clientsocket > 0)
		{
			/* Found an open client ... speak with him */
			if (process_client(server, &(server->client[i]) ) == FCSERVER_RET_CLOSED)
			{
				DEBUG_PLINE("Client with socket %d closed", server->client[i].clientsocket);
				if (server->onClientChange > 0)
				{
					server->onClientChange(server->clientcount, 
										   FCCLIENT_STATUS_DISCONNECTED, 
										   server->client[i].clientsocket);
				}
				hwal_memset( &(server->client[i]), 0, sizeof(fcclient_t) );
				server->clientcount--;				
			}
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
	
	if (server->imageBuffer > 0)
	{
		hwal_free(server->imageBuffer);
	}
	
	return FCSERVER_RET_OK;
}

fcserver_ret_t fcserver_setactive (fcserver_t* server, int status)
{
	if (server == NULL)
		return FCSERVER_RET_PARAMERR;
	
	if (status > 0)
	{
		server->status = 1;
	}
	else
	{
		server->status = 0;
	}	
	return FCSERVER_RET_OK;
}
