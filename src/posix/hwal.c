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
#include "../hwal.h"

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

extern void hwal_memcpy(const void *s1, const void* s2, int length)
{
	memcpy(s1, s2, length);
}
