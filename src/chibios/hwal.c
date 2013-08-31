/*
 *  hwal.c
 *
 * Hardware abstraction layer
 * ==========================
 *
 * Implementation for Chibios
 * 
 *  Created by ollo on 30.08.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <string.h>

#include "ch.h"

#include "hwal.h"

#include "ff.h"

#define MAXFILEDESCRIPTORS	10

/* a small mapping table to abstract from the filedescriptors */
FIL*	fd_mappingtable[MAXFILEDESCRIPTORS];
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
	/*FIXME only allow reading for an easy handling */
	if ( f_open( fd_mappingtable[usedMapIndex - 1], (TCHAR*) filename, FA_READ) != FR_OK)
	{
		fd_mappingtable[usedMapIndex -1] = NULL;
		return 0; /* damn it did not work */
	}
	
	return usedMapIndex;
}
	
extern int hwal_fread(void* buffer, int length, int filedescriptor)
{
	int br;
	if (f_read( fd_mappingtable[filedescriptor -1], (TCHAR*) buffer, length,(UINT*) &br) != FR_OK)
	{
		return 0; /* problems, return zero as problematic length */
	}
	else
	{
		return br; /* return the number of read bytes */
	}
}
	
	
extern void hwal_fclose(int filedescriptor)
{
	f_close( fd_mappingtable[filedescriptor -1] );
	/* FIXME free the entries in the mapping table */
}

extern void hwal_memset(void *buffer, int item, int length)
{
	int i;
	uint8_t* b = (uint8_t*) buffer;
	for(i=0; i < length; i++)
	{
		b[i] = item;
	}
}

extern void hwal_memcpy(const void *s1, const void* s2, int length)
{
	int i;
	uint8_t* buffs1 = (uint8_t*) s1;
	uint8_t* buffs2 = (uint8_t*) s2;
	for(i=0; i < length; i++)
	{
		buffs1[i] = buffs2[i];
	}	
}

extern int hwal_strlen(char* text)
{
	int i =0;

	while (text[i] != '\0') {
		i++;
	}
	return i;
}