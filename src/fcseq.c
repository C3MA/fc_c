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
#include "hwal.h"


#define FCSEQ_TMPBUFFER_HEAD_SIZE       6       /* length of the internal buffer to read the length */
#define NULL 				0 	/* needed but was in removed <string.h> */

/******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************/

fcseq_ret_t extractFrame(uint8_t* memory, uint8_t* rgb24, int width, int offset, int length);

/******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************/

#ifndef FCSEQBUFFER_ONLY

fcseq_ret_t fcseq_load(char *filename, fcsequence_t* seq)
{
	/* check if the file exists */
	int pFile;
	
	if (seq == NULL)
	{
		return FCSEQ_RET_PARAMERR;
	}
	

	/* clean the given a structure for the sequence */
	hwal_memset(seq, 0, sizeof(fcsequence_t));
	seq->type = FCSEQ_FILEDESCRIPTOR;
	
	seq->intern.file.filedescriptor = hwal_fopen(filename, "r");
	
	if ( seq->intern.file.filedescriptor == 0 )
	{
		return FCSEQ_RET_IOERR;	
	}
	
	hwal_debug(__FILE__, __LINE__, "Open file on descriptor %d", seq->intern.file.filedescriptor);

	/* Read the beginning of the file */
	uint8_t mem[FCSEQ_TMPBUFFER_HEAD_SIZE];
	int read = hwal_fread(mem, FCSEQ_TMPBUFFER_HEAD_SIZE, seq->intern.file.filedescriptor);
	hwal_debug(__FILE__, __LINE__, "FD%d returned %d bytes (asking for %d)", seq->intern.file.filedescriptor, read, FCSEQ_TMPBUFFER_HEAD_SIZE);

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
    
    offset = parse_number(memory, offset, &size); // Read length of the header (containing meta information)
    if (seqio->type == FCSEQ_FILEDESCRIPTOR)
    {
	/* The already memory may not contain the complete meta information */
	int restOfFirst = (length - offset);
	
	/* copy the already read information into a buffer */
	uint8_t metabuffer[size];
	hwal_memcpy(metabuffer, memory + offset, restOfFirst);
	/* the further necessary bytes are read from the file now */
	int readBytes = hwal_fread(metabuffer+restOfFirst, size - restOfFirst, seqio->intern.file.filedescriptor);
	if ( readBytes != (size-restOfFirst) )
	{	/* big problem! there were not enough bytes in the file */
		return FCSEQ_RET_IOERR;
	}
    	(void) parse_metadata(metabuffer, 0, &(seqio->fps), &(seqio->width), &(seqio->height), NULL, NULL);
	
    }
    else
    {
    	offset = parse_metadata(memory, offset,&(seqio->fps), &(seqio->width), &(seqio->height), NULL, NULL);
	if (offset == -1)
	{
           /* on problems, leave the offset at the beginning */
	   return FCSEQ_RET_INVALID_DATA;
	}
	
	/* update the offset */
	seqio->intern.mem.actOffset = offset;
    }	
	return FCSEQ_RET_OK;
}

fcseq_ret_t fcseq_nextFrame(fcsequence_t* seqio, uint8_t* rgb24)
{
	int offset = 0;
	int id, type, frame_length;
	fcseq_ret_t ret;

	if (seqio->type == FCSEQ_FILEDESCRIPTOR) /* reading a frame from a file */
	{
		uint8_t* memFrame;
		
		/* Read the beginning of the file */
		uint8_t mem[FCSEQ_TMPBUFFER_HEAD_SIZE];
		int read = hwal_fread(mem, FCSEQ_TMPBUFFER_HEAD_SIZE, seqio->intern.file.filedescriptor);
		
		hwal_debug(__FILE__, __LINE__, "Read from a file");
		
		/* check that all requested data was read */
		if (read != FCSEQ_TMPBUFFER_HEAD_SIZE)
		{
			/* when the header was not complete, where should be the user data? */
			return FCSEQ_RET_INVALID_DATA;
		}

		/****** Read frame header *******/
		offset = parse(mem, offset, &id, &type);
		hwal_debug(__FILE__, __LINE__, "Header at %d with id=%d and type=%d", offset, id, type);

		if (id == BINARYSEQUENCE_FRAME && type == PROTOTYPE_LENGTHD && offset > -1)
		{
			offset = parse_number(mem, offset, &frame_length);
			hwal_debug(__FILE__, __LINE__, "%d is the size of the actual frame", frame_length );
		}
		else
		{
			hwal_debug(__FILE__, __LINE__, "Header at %d with id=%d and type=%d was NOT %d and type %d", BINARYSEQUENCE_FRAME, PROTOTYPE_LENGTHD);
			/* no header -> end of file */
			return FCSEQ_RET_EOF;
		}
		
		memFrame = (uint8_t*) hwal_malloc(frame_length);
		if (memFrame == 0)
		{
			hwal_debug(__FILE__, __LINE__, "Requested for %d bytes and got %x", frame_length, memFrame);
			return FCSEQ_RET_OUTOFMEMORY;
		}
		
		/* The already memory may not contain the complete meta information */
		int restOfFirst = (FCSEQ_TMPBUFFER_HEAD_SIZE - offset);
		hwal_debug(__FILE__, __LINE__, "%x -> %x length=%d", memFrame, mem + offset, restOfFirst);
		/* copy the already read information into a buffer */
		hwal_memcpy(memFrame, mem + offset, restOfFirst);

		/* the further necessary bytes are read from the file now */
		int readBytes = hwal_fread((uint8_t *) (memFrame + restOfFirst), frame_length - restOfFirst, 
					seqio->intern.file.filedescriptor);
		if ( readBytes != (frame_length - restOfFirst) )
		{	/* big problem! there were not enough bytes in the file */
			hwal_debug(__FILE__, __LINE__, "Could not find %d bytes for the next frame", frame_length - restOfFirst);
			return FCSEQ_RET_IOERR;
		}	
		hwal_debug(__FILE__, __LINE__, "Now start the parsing from the memory");

		ret = extractFrame(memFrame, rgb24, seqio->width, 0, frame_length);
		hwal_free(memFrame);
		return ret;
	}
	else	/* when working directly in the memory of the complete structure */
	{
		offset = seqio->intern.mem.actOffset;

		/****** Read frame header *******/
		offset = parse(seqio->intern.mem.pBuffer, offset, &id, &type);	
		if (id == BINARYSEQUENCE_FRAME && type == PROTOTYPE_LENGTHD && offset > -1)
		{
			offset = parse_number(seqio->intern.mem.pBuffer, offset, &frame_length);
		}
		else
		{
			/* no header -> end of file */
			return FCSEQ_RET_EOF;
		}
		return extractFrame(seqio->intern.mem.pBuffer, rgb24, seqio->width, offset, frame_length);
	
		/* update the offset */
		seqio->intern.mem.actOffset = offset + frame_length;
	}
}


fcseq_ret_t extractFrame(uint8_t* memory, uint8_t* rgb24, int width, int offset, int length)
{
	int lastOffset = offset + length;
	int red, green, blue;
	int x,y, pos;

	if (memory == NULL || rgb24 == NULL || length == 0)
	{
		hwal_debug(__FILE__, __LINE__, "Problem on the parameters: memory: %x, rgb24: %x, length: %d", 
				   memory, rgb24, length);
		return FCSEQ_RET_PARAMERR;
	}

	/***** extract all stored pixel *****/
	do {
		offset = frame_parse_pixel(memory, offset, &red, &green, &blue, &x, &y);
		if (offset == -1) {
			return FCSEQ_RET_INVALID_DATA;
		} else {
			/* printf("Parse Pixel, red: %2x , green: %2x , blue: %2x x: %3d y: %3d\n",red,green,blue,x,y); */
			/* pick row             and colum (times three, ass each color needs one byte */
			pos = y*(width*3) + (3*x);
			rgb24[pos + 0] = red;
			rgb24[pos + 1] = green;
			rgb24[pos + 2] = blue;
		}
	} while (offset < lastOffset);
	
	return FCSEQ_RET_OK;
}
