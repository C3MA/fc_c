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
	int hwal_fopen(char *filename, char* type);
	
	/**
	 * Read from the file into given buffer
	 * 
	 * @param[out]	buffer	to store the read content in
	 * @param[int]	length	amount of bytes that should be read
	 * @param[in]	filedescriptor	unique number for each opened file (given from the hwal_open function)
	 * @return amount of bytes that were actual read
	 */
	int hwal_fread(void* buffer, int length, int filedescriptor);
	
	
	/**
	 * Free all used memory, when a file was used as input
	 * @param[in] filedescriptor	the unique number for each opened file
	 */
	void hwal_fclose(int filedescriptor);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif
