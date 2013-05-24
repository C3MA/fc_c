/*
 *  fcseq.c
 *
 *  Created by ollo on 20.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "fcseq.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef FCSEQBUFFER_ONLY

fcsequence_t* fcseq_load(char *filename)
{
	/* check if the file exists */
	FILE * pFile;
	int filesize;
	uint8_t* memory;
	
	pFile = fopen (filename,"r");
	if (pFile==NULL)
	{
		return NULL;
	}
	
	/* generate a structure containing the sequence */
	fcsequence_t* seq = malloc(sizeof(fcsequence_t));
	memset(seq, 0, sizeof(fcsequence_t));
	seq->type = FCSEQ_FILEDESCRIPTOR;
	
	fseek(pFile, 0, SEEK_END); // seek to end of file
	filesize = ftell(pFile); // get current file pointer
	fseek(pFile, 0, SEEK_SET); // seek back to beginning of file
	
	if (filesize <= 0)
	{
		return NULL;
	}
	
	/* copy the complete file into the memory */
	memory = malloc(filesize);
	fread(memory, 1, filesize, pFile);
	fclose(pFile);
	
	fcseq_loadMemory(seq, memory, filesize);
	
	return seq;
}

void fcseq_close(fcsequence_t* seq)
{
	/* only on a suitable memory, we can operate*/
	if (seq == NULL)
	{
		return;
	}
	
	/* only, when a file was used as soure,
	 * this module allocates memory, which must be freed 
	 */
	if (seq->type == FCSEQ_FILEDESCRIPTOR)
	{
		free(seq->pBuffer);
		free(seq);
	}
}

#endif

fcseq_ret_t fcseq_loadMemory(fcsequence_t* seqio, uint8_t *memory, uint32_t length)
{
	if (seqio == NULL)
	{
		return FCSEQ_RET_PARAMERR;
	}
	
	/* when no source was specified, guess the memory as origin */
	if (seqio->type != FCSEQ_FILEDESCRIPTOR)
	{
		seqio->type = FCSEQ_MEMORY;
	}
	
	/* initialize memory stucture */
	seqio->bufferLength = length;
	seqio->pBuffer = memory;
	
	/* load the header of the file */
	
	return FCSEQ_RET_NOTIMPL;
}

fcseq_ret_t fcseq_nextFrame(fcsequence_t* seqio, uint8_t* rgb24)
{
	return FCSEQ_RET_NOTIMPL;
}
