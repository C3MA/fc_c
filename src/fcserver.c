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
