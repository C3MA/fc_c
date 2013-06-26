/*
 *  fcseq.h
 *
 *  Created by ollo on 20.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

/*! @addtogroup fcseq
 *
 * @details This module parses an sequence file
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
		FCSEQ_RET_PARAMERR, /* invalid parameter were used (NULL pointer) */
		FCSEQ_RET_NOTIMPL, /* not implementend atm */
		FCSEQ_RET_INVALID_DATA
	};
	typedef enum FCSEQ_RET fcseq_ret_t;
	
	
	enum FCSEQ_TYPE { 
		FCSEQ_NONE = 0, 
		FCSEQ_FILEDESCRIPTOR = 1, 
		FCSEQ_MEMORY = 2
	};
	
	typedef enum FCSEQ_TYPE fcseqtype_t;
	

	/** @struct FCSEQ_MEM 
	 *  @brief specific variables needed for working with the memory (used in @see FCSEQ) */
	struct FCSEQ_MEM {
		uint8_t*	pBuffer; /**< pointer to the memory, containing the protobuf sequence */
		uint32_t	bufferLength; /**< The length of the given memory. */
		uint32_t	actOffset;	/**< actual position in the memory buffer */
	};

	/** @struct FCSEQ_FILE
	 * @brief specific variables needed for files (used in @see FCSEQ) */
	struct FCSEQ_FILE {
		int	filedescriptor; /**< the used filedescriptor defined in the abstraction layer: hwal.h */
	};

	/** @struct FCSEQ
	 *  @brief This structure contains meta information of a sequence and the needed filepointer
	 *  @var FCSEQ::width 
	 *  Member 'width' contains the width of the wall
	 *  @var FCSEQ::height 
	 *  Member 'height' contains the height of the wall
	 *  @var FCSEQ::fps 
	 *  Member 'fps' contains the expected frames per second (that is ignored when using the network protocol)
	 *
	 *  @var FCSEQ::type 
	 *  Member 'type' defines the source
	 */
	struct FCSEQ {
		int width;
		int height;
		int fps;
		union {
			struct FCSEQ_MEM mem; /**< information about the used memory */
			struct FCSEQ_FILE file; /**< information about the used file */
		} intern; /**< working structure, ether a file or the memory is used. */	
		fcseqtype_t 	type;
	};
	
	typedef struct FCSEQ fcsequence_t;

#ifndef FCSEQBUFFER_ONLY
	
	/* Create a new sequence.
	 * @brief The given filename is opened and the meta information is extracted.
	 * YOU must CLOSE the file after the usage with 'fcseq_close' !
	 *
	 * @param filename the name or path of the file to load
	 * @param seq the header with some meta information
	 * @return fcseq_ret_t status of the opening procedure
	 */
	fcseq_ret_t fcseq_load(char *filename, fcsequence_t* seq);
	
	
	/**
	 * Close the file
	 * @param[in] seq
	 */
	void fcseq_close(fcsequence_t* seq);
	
#endif
	
	/* Initialize a sequence and Parse meta information from a new sequence.
	 *
	 * @param[out] seqio to fill
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
