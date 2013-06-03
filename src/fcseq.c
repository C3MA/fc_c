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
#include "hwal.h"


#define FCSEQ_TMPBUFFER_HEAD_SIZE       6       /* length of the internal buffer to read the length */

#ifndef FCSEQBUFFER_ONLY

fcseq_ret_t fcseq_load(char *filename, fcsequence_t* seq)
{
	/* check if the file exists */
	FILE * pFile;
	int filesize;
	uint8_t* memory;
	
	pFile = fopen (filename,"r");
	if (pFile==NULL)
	{
		return FCSEQ_RET_IOERR;
	}
	
	if (seq == NULL)
	{
		return FCSEQ_RET_PARAMERR;
	}
	

	/* clean the given a structure for the sequence */
	memset(seq, 0, sizeof(fcsequence_t));
	seq->type = FCSEQ_FILEDESCRIPTOR;
	
	seq->intern.file.filedescriptor = hwal_fopen(filename, "r");
	
	if ( seq->intern.file.filedescriptor == NULL )
	{
		return FCSEQ_RET_IOERR;	
	}

	/* Read the beginning of the file */
	uint8_t mem[FCSEQ_TMPBUFFER_HEAD_SIZE];
	int read = hwal_fread(mem, FCSEQ_TMPBUFFER_HEAD_SIZE, seq->intern.file.filedescriptor);

	fprintf(stderr, "Debug %d and returned length\n", FCSEQ_TMPBUFFER_HEAD_SIZE, read);
	/* check that all requested data was read */
	if (read != FCSEQ_TMPBUFFER_HEAD_SIZE)
	{
		/* when the header was not complete, where should be the user data? */
		return FCSEQ_RET_INVALID_DATA;
	}

	return fcseq_loadMemory(seq, mem, FCSEQ_TMPBUFFER_HEAD_SIZE); 
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
		hwal_fclose(seq->intern.file.filedescriptor);
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
		/* initialize memory stucture */
		seqio->intern.mem.bufferLength = length;
		seqio->intern.mem.pBuffer = memory;
		
		/* load the header of the file */
		seqio->intern.mem.actOffset = 0;
	}
	
	
	/* verification if the image is the expected */
    offset = parse(memory, offset, &id, &type);
    if (id != BINARYSEQUENCE_METADATA || type != PROTOTYPE_LENGTHD)
        return FCSEQ_RET_INVALID_DATA; /* on problems, leave the offset at the beginning */
    
    offset = parse_number(memory, offset, &size); // Read length of req_snip
    if (seqio->type == FCSEQ_MEMORY )
    {		
	/* FIXME the read size can be compared with the available length (detect compromised files here) */
    }
	
    offset = parse_metadata(memory, offset,&(seqio->fps), &(seqio->width), &(seqio->height), NULL, NULL);
	if (offset == -1) {
		/* on problems, leave the offset at the beginning */
		return FCSEQ_RET_INVALID_DATA;
	}
	
	/* update the offset */
	seqio->intern.mem.actOffset = offset;
	
	return FCSEQ_RET_OK;
}

fcseq_ret_t fcseq_nextFrame(fcsequence_t* seqio, uint8_t* rgb24)
{
	int offset = seqio->intern.mem.actOffset;
	int id, type;
	int frame_offset, frame_length, frame_offset_start;
	int red, green, blue;
	int x,y, pos;
	
	/****** Read frame header *******/
    offset = parse(seqio->intern.mem.pBuffer, offset, &id, &type);	
    if (id == BINARYSEQUENCE_FRAME && type == PROTOTYPE_LENGTHD && offset > -1)
    {
        offset = parse_number(seqio->intern.mem.pBuffer, offset, &frame_length);
        frame_offset = offset;
    }
    else
    {
		/* no header -> end of file */
        return FCSEQ_RET_EOF;
    }
		
	/***** extract all stored pixel *****/
	frame_offset_start = frame_offset;
	do {
		frame_offset = frame_parse_pixel(seqio->intern.mem.pBuffer,frame_offset, &red, &green, &blue, &x, &y);
		if (offset == -1) {
			return FCSEQ_RET_INVALID_DATA;
		} else {
			/* printf("Parse Pixel, red: %2x , green: %2x , blue: %2x x: %3d y: %3d\n",red,green,blue,x,y); */
			/* pick row             and colum (times three, ass each color needs one byte */
			pos = y*(seqio->width*3) + (3*x);
			rgb24[pos + 0] = red;
			rgb24[pos + 1] = green;
			rgb24[pos + 2] = blue;
		}
	} while (frame_offset < (frame_offset_start+frame_length));
	
	
	/* update the offset */
	seqio->intern.mem.actOffset = frame_offset;
	
	return FCSEQ_RET_OK;
}
