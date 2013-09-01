/*
 *  customHwal.h
 *
 * Hardware abstraction layer
 * ==========================
 *
 * Some special features of the hardware abstraction layer
 *
 *  Created by ollo on 20.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#ifndef CUSTOM_HWAL_H
#define CUSTOM_HWAL_H

#include "chprintf.h"

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
	
	/*
	 * Initialiaztion
	 * @param[in]	codefile	the file, where the message comes frome
	 * @param[in]	linenumber	the excat line of code of this code
	 * @param[in]	text		to display
	 */
	extern void hwal_init(BaseSequentialStream *chp);
	
#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif
