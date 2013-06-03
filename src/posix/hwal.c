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

int* hwal_open(char *filename, char* type)
{
	return fopen(filename, type);	
}
	
int hwal_read(void* buffer, int length, int* filedescriptor)
{
	return fread(buffer, 1, length, (FILE*) filedescriptor);
}
	
	
void hwal_close(int* filedescriptor)
{
	fclose((FILE*) filedescriptor);
}

