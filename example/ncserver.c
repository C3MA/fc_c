/* @file ncserver.c
 * @brief Server simulation with an output via ncurses
 * @author ollo
 *
 * @date 15.12.2013 – Created this module
 * @defgroup ncurses_server
 * 
 * Copyright 2013 C3MA. All rights reserved.
 */
#include <stdio.h>
#include "fcserver.h"

void printUsage()
{
	printf("Usage: <width> <height>");
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printUsage();
	}
}
