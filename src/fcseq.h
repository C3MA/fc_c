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

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
	
	enum FCSEQ_RET { FCSEQ_RET_OK=1, 
		FCSEQ_RET_IOERR,
		FCSEQ_RET_EOF
	};
	typedef enum FCSEQ_RET fcseq_ret_t;
	
	
	enum FCSEQ_TYPE { FCSEQ_FILEDESCRIPTOR=1, 
		FCSEQ_MEMORY=2
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
		
		FILE fd;
		uint8_t* buffer;
		fcseqtype_t type;
	};
	
	typedef struct FCSEQ fcsequence_t;

	/**
	 * Create a new sequence.
	 * the given filename is opened and the meta information is extracted.
	 *
	 * @param[in] filename the name of the file
	 *
	 * @return The new generated sequence.
	 * YOU must FREE the memory yourself after usage!
	 */
	fcsequence_t* load(char *filename);
	

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif