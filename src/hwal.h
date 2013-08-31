/*
 *  hwal.h
 *
 * Hardware abstraction layer
 * ==========================
 *
 * Abstraction for the following functionality
 * - opening files
 * - read from file into an buffer
 *
 *  Created by ollo on 20.05.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#ifndef HWAL_H
#define HWAL_H

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
	
	/**
         * Open an file.
	 * An interface to abstract to the plattform dependent code to open an file
	 *
	 * @param[in]	filename the name with or without the complete path
	 * @param[in]	type	additional information like 'r' for read, 'w' for writing access and so on
	 * @return unique number, for each opened file
	 */
	extern int hwal_fopen(char *filename, char* type);
	
	/**
	 * Read from the file into given buffer
	 * 
	 * @param[out]	buffer	to store the read content in
	 * @param[int]	length	amount of bytes that should be read
	 * @param[in]	filedescriptor	unique number for each opened file (given from the hwal_open function)
	 * @return amount of bytes that were actual read
	 */
	extern int hwal_fread(void* buffer, int length, int filedescriptor);
	
	
	/**
	 * Free all used memory, when a file was used as input
	 * @param[in] filedescriptor	the unique number for each opened file
	 */
	extern void hwal_fclose(int filedescriptor);
	
	
	/**
	 * Fill the Memory with the given value.
	 * 
	 * @param[in|out]	buffer	to be filled
	 * @param[int]	length	value that should be written into each item
	 * @param[int]	length	amount of bytes that should be read
	 */
	extern void hwal_memset(void *buffer, int item, int length);
	
	/**
	 * The memcpy() function copies n bytes from memory area s2 to memory area s1.
	 * If s1 and s2 overlap, behavior is undefined.  Applications in which s1 and s2
     * might overlap should use memmove(3) instead.
	 * 
	 * @param[out]	s1	to be filled
	 * @param[int]	s2	memory area with the information to read
	 * @param[int]	length	amount of bytes to be copied
	 */
	extern void hwal_memcpy(const void *s1, const void* s2, int length);
	
	/**
	 * Calculate the length of the given string
	 * @param[int]	text	the length should be calculated from
	 * @return calculated length for the given text
	 */
	extern int hwal_strlen(char* text);
#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif
