/*
 *  fcseq.h
 *
 *  Created by ollo on 20.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#ifndef FC_SEQUENCE_PARSER_H
#define FC_SEQUENCE_PARSER_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
	
	enum FCSEQ_RET { FCSEQ_RET_OK=1, 
		FCSEQ_RET_IOERR,
		FCSEQ_RET_EOF,
		FCSEQ_RET_PARAMERR,
		FCSEQ_RET_NOTIMPL /* not implementend atm */
	};
	typedef enum FCSEQ_RET fcseq_ret_t;
	
	
	enum FCSEQ_TYPE { 
		FCSEQ_NONE = 0, 
		FCSEQ_FILEDESCRIPTOR = 1, 
		FCSEQ_MEMORY = 2
	};
	
	typedef enum FCSEQ_TYPE fcseqtype_t;
	
	/** @struct FCSEQ Definition file to parse
	 *  @brief This structure contains meta information of a sequence and the needed filepointer
	 *  @var FCSEQ::width 
	 *  Member 'width' contains the width of the wall
	 *  @var FCSEQ::height 
	 *  Member 'height' contains the height of the wall
	 *  @var FCSEQ::fps 
	 *  Member 'fps' contains the expected frames per second (that is ignored when using the network protocol)
	 *
	 *  @var FCSEQ::fd 
	 *  Member 'fd' the FILE, that is used
	 */
	struct FCSEQ {
		int width;
		int height;
		int fps;
		
		uint8_t* pBuffer;
		uint32_t bufferLength;
		fcseqtype_t type;
	};
	
	typedef struct FCSEQ fcsequence_t;

#ifndef FCSEQBUFFER_ONLY
	
	/**
	 * Create a new sequence.
	 * the given filename is opened and the meta information is extracted.
	 *
	 * @param[in] filename the name of the file
	 *
	 * @return The new generated sequence.
	 * YOU must FREE the memory after usage with 'fcseq_close' !
	 */
	fcsequence_t* fcseq_load(char *filename);
	
	
	/**
	 * Free all used memory, when a file was used as input
	 * @param[in] seq
	 */
	void fcseq_close(fcsequence_t* seq);
	
#endif
	
	/**
	 * Initialize a sequence and Parse meta information from a new sequence.
	 *
	 * @param[out] seqio to fill
	 *
	 * @param[in] memory with the sequence
	 * @param[in] length of the available memory containing the sequence
	 *
	 * @return status
	 */
	fcseq_ret_t fcseq_loadMemory(fcsequence_t* seqio, uint8_t *memory, uint32_t length);
	
	/* Extract one frame from the sequence.
	 *
	 * @param[in] seqio the structure of the sequence that should be parsed
	 * @param[out] rgb24 memory buffer, the files should be stored in.
	 * The USER has to provide enough space for this memory buffer!
	 *
	 * @return status when reading the frame
	 * - FCSEQ_RET_EOF (there are no more frames stored in the sequence file)
	 */
	fcseq_ret_t fcseq_nextFrame(fcsequence_t* seqio, uint8_t* rgb24);
	

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif