/*
 *  hwalNet.c
 *
 *  Created by ollo on 01.11.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 *   TCP Socket implementation in a seperate thread, handling the accept-logic
 */

#include "hwal_net.h"

/* Socket requirements */
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#define HWALNET_MAILBOX_SIZE		10

static WORKING_AREA(wa_hwalnet_tcp, HWAL_NET_THREAD_STACK_SIZE);


/* Mailbox, filled by the fc_server thread */
static uint32_t buffer4mailbox[HWALNET_MAILBOX_SIZE];
static MAILBOX_DECL(hwalNetmailbox, buffer4mailbox, HWALNET_MAILBOX_SIZE);

/* Static variables, that are initialized before the thread is started
 * and are needed for this thread */
static int gConnection = 0;

	msg_t hwal_background(void *p)
	{
		struct netconn *newconn;
		struct netconn *conn = (struct netconn *) gConnection;
		
		chRegSetThreadName("TCPaccept");
		(void)p;
		
		/* Prepare Mailbox to communicate with the others */
		chMBInit(&hwalNetmailbox, (msg_t *)buffer4mailbox, HWALNET_MAILBOX_SIZE);
		
		while (TRUE)
		{	
			netconn_accept(conn, &newconn);
			chMBPostI(&hwalNetmailbox, (uint32_t) newconn);
		}
		
	}
	
	int	hwalnet_new_socket(int port, int maximumClients)
	{
		struct netconn *conn;
		if (gConnection)
		{
			DEBUG_PLINE("The thread was already initialized");
			return -1;
		}
		
		/* Create a new TCP connection handle */
		conn = netconn_new(NETCONN_TCP);
		
		/* Bind to port with default IP address */
		netconn_bind(conn, NULL, port);
		
		/* Put the connection into LISTEN state */
		netconn_listen(conn);
		
		/* Make the communication noneblocking */
		//netconn_set_nonblocking(conn, TRUE);
		
		gConnection = (int) conn;
		
		/*
		 * Creates an thread, handling the accept
		 */
		chThdCreateStatic(wa_hwalnet_tcp, sizeof(wa_hwalnet_tcp), NORMALPRIO + 1,
						  hwal_background, NULL);
		
		DEBUG_PLINE("Created new thread for Socket %d and Port %d", gConnection, port);
		return gConnection;
	}	
	
	
	int hwalnet_new_client( int socketConnection )
	{
		msg_t msg1, status;
		int newMessages;
		uint32_t newClient = -1;
		
		/* Use nonblocking function to count incoming messages */
		newMessages = chMBGetUsedCountI(&hwalNetmailbox);
		
		//DEBUG_PLINE("open accepts %d", newMessages);
		
		if (newMessages)
		{
			status = chMBFetch(&hwalNetmailbox, &msg1, TIME_INFINITE);
		
			/*Check the TCP-socket, is the correct one that is asked for with 'socketConnection' */
			if (socketConnection != gConnection)
			{
				return -2;
			}
			
			if (status == RDY_OK)
			{
				chSysLock();
				newClient = (uint32_t) msg1;
				chSysUnlock();
			}		
			
			DEBUG_PLINE("Got new client: %d", newClient);
		}
		return (int) newClient;
	}
	
