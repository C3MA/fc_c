/* @file util.h
 * @brief Some usefull functions, reimplemented to be plattform independent
 * @author Ollo
 *
 * @defgroup LowlevelFC
 *
 */


#ifndef fc_c_util
#define fc_c_util

/** @addtogroup LowlevelFC */
/*@{*/

#include <stdint.h>

#define NULL 0

int fc_util_itoa_rightalign(char* buffer, uint32_t width, int32_t number);

int fc_util_atoi_rightalign(char* buffer, uint32_t width, int32_t* number);

/*@}*/

#endif
