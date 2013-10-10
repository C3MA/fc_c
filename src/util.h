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

#ifndef NULL
#define NULL 0
#endif

int fc_util_itoa_rightalign(char* pBuffer, uint32_t width, int32_t number);

int fc_util_atoi_rightalign(char* pBuffer, uint32_t width, int32_t* pNumber);

/*@}*/

#endif
