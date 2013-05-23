/*
 *  parsefile.c
 *
 *  Created by ollo on 21.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <stdio.h>
#include "fcseq.h"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "usage %s <sequence file>\n", argv[0]);
		return 1;
	}
	
	fcsequence_t* seq = fcseq_load(argv[1]);
	
	if (seq == NULL)
	{
		fprintf(stderr, "Unable to load sequnce (%s:%d)", __FILE__, __LINE__);
	}
	
	return 0;
}