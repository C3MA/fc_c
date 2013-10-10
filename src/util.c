#include "util.h"


#define DIGIT_BASE 10
#define ASCII_PREFIX ' '
#define ASCII_0		 '0'

static int fc_util_pow(int base, int exponent)
{
	int number = 1;
	
	if (exponent <= 0) {
		return number;
	}
	
	for (; exponent > 0; exponent--) {
		number = number * base;
	}
	return number;
}

/**
 * @param[in|out] pBuffer already allocated with the minimum size of "width
 * @param[in] width	the number should be aligned
 * @param[in] number	to write into the buffer
 * @return width of the digits, or "zero" on error
 */
int fc_util_itoa_rightalign(char* pBuffer, uint32_t width, int32_t number)
{
	int tmp, length, prefix, i = 0;
	int num = number;
	
	/* calculate the amount of digits */	
	for(length = 0;num !=0; num = num / 10)
	{
		length++;
	}
	prefix = width - length;
	
	/* secure, that the expected size is large enough */
	if (length > width)
	{
		return 0;
	}
	
	/* Fill the prefix, to create a right aligned number */
	for (i=0; i < prefix; i++) {
		pBuffer[i] = ASCII_PREFIX;
	}

	/* Convert the number into its ASCII representive */
	for (i=length; i > 0; i--)
	{
		tmp = (number % fc_util_pow(DIGIT_BASE, i));
		tmp = tmp / fc_util_pow(DIGIT_BASE, i - 1);
		pBuffer[prefix + (length - i)] = ASCII_0 + tmp;
	}
	
	return length;
}

/**
 * @param[in] pBuffer with the ASCII number
 * @param[in] width the number should be aligned
 * @param[in] pNumber pointer to an already allocated variable
 * @return width of the digits, or "zero" on error
 */
int fc_util_atoi_rightalign(char* pBuffer, uint32_t width, int32_t* pNumber)
{
	return 0;
}
