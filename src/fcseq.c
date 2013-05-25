/*
 *  fcseq.c
 *
 *  Created by ollo on 20.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include "fcseq.h"
#include "fc.h"
#include "proto.h"
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
	int offset = 0;
	
	int id, type, size;
	
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
	seqio->actOffset = 0; 
	
	/* verification if the image is the expected */
    offset = parse(seqio->pBuffer, seqio->actOffset, &id, &type);
    if (id != BINARYSEQUENCE_METADATA || type != PROTOTYPE_LENGTHD)
        return FCSEQ_RET_INVALID_DATA; /* on problems, leave the offset at the beginning */
    
    offset = parse_number(seqio->pBuffer, offset, &size); // Read length of req_snip
	/* FIXME the read size can be compared with the available length (detect compromised files here) */
	
	offset = parse_metadata(seqio->pBuffer,offset,&(seqio->fps), &(seqio->width), &(seqio->height), NULL, NULL);
	if (offset == -1) {
		/* on problems, leave the offset at the beginning */
		return FCSEQ_RET_INVALID_DATA;
	}
	
	/* update the offset */
	seqio->actOffset = offset;
	
	return FCSEQ_RET_OK;
}

fcseq_ret_t fcseq_nextFrame(fcsequence_t* seqio, uint8_t* rgb24)
{
	int offset = seqio->actOffset;
	int id, type;
	int frame_offset, frame_length, frame_offset_start;
	int red, green, blue;
	int x,y;
	
	// Read Frames
    
    offset = parse(seqio->pBuffer, offset, &id, &type);
	
    if (id == BINARYSEQUENCE_FRAME && type == PROTOTYPE_LENGTHD && offset > -1)
    {
        offset = parse_number(seqio->pBuffer, offset, &frame_length);
        frame_offset = offset;
    }
    else
    {
        return FCSEQ_RET_INVALID_DATA;
    }
		
	/* extract all stored pixel */
	frame_offset_start = frame_offset;
	do {
		frame_offset = frame_parse_pixel(seqio->pBuffer,frame_offset, &red, &green, &blue, &x, &y);
		if (offset == -1) {
			printf("parse Pixel faild\n");
			return FCSEQ_RET_NOTIMPL;
		} else {
			printf("Parse Pixel, red: %2x , green: %2x , blue: %2x x: %3d y: %3d\n",red,green,blue,x,y);
		}
	} while (frame_offset < (frame_offset_start+frame_length));
	
	
	/* update the offset */
	seqio->actOffset = offset;
	
	return FCSEQ_RET_OK;
}
