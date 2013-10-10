
#include "util.h"

#ifndef _CHIBIOS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
/* when using chibios */
#define NULL 0
#endif

/**
 * @param[in|out] buffer already allocated with the minimum size of "width
 * @param[in] width	the number should be aligned
 * @param[in] number	to write into the buffer
 * @return width of the digits, or "zero" on error
 */
int fc_util_itoa_rightalign(char* buffer, uint32_t width, int32_t number)
{
	return 0;
}

/**
 * @param[in] buffer with the ASCII number
 * @param[in] width the number should be aligned
 * @param[in] number pointer to an already allocated variable
 * @return width of the digits, or "zero" on error
 */
int fc_util_atoi_rightalign(char* buffer, uint32_t width, int32_t* number)
{
	return 0;
}
