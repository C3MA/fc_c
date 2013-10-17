/*
 *  fcserver.c
 *
 *  Created by ollo on 16.10.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "fcserver.h"
#include "fcseq.h"
#include "fc.h"
#include "hwal.h"

/******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************/


/******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************/


fcserver_ret_t fcserver_init (fcserver_t* server, ImageCallback_t onNewImage,
							 ClientCallback_t onNewClient)
{
	
#if 0
	struct sockaddr_in srcAddress;
	
	struct sockaddr_in  srcAddr;
	socklen_t           sockLen = sizeof(srcAddr);
	
    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return FCSERVER_RET_IOERR;
    }
#endif
	
	return FCSERVER_RET_NOTIMPL;
}

fcserver_ret_t fcserver_process (void)
{
	return FCSERVER_RET_NOTIMPL;
}

fcserver_ret_t fcserver_close (void)
{
	return FCSERVER_RET_NOTIMPL;
}
