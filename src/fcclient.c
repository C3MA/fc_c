/*
 *  fcclient.c
 *  XCodeFC_C
 *
 *  Created by ollo on 24.04.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "fcclient.h"
#include <string.h>

/*
 * creation of a new connection
 * allocating new memory.
 * YOU have to free this memory after using!
 */
extern fcclient_t* fclient_new()
{
	fcclient_t* tmp = malloc(sizeof(fcclient_t));
	memset(tmp, 0, sizeof(fcclient_t));
	return tmp;
}
