/**
 * @file hwal.h
 * @brief Hardware abstraction layer
 * @author Ollo
 *
 * @date 20.05.2013 – Started with memory and filesystem abstraction
 * @date 16.10.2013 - Started with network functionality
 * @defgroup Abstraction
 *
 * Abstraction for the following functionality
 * - opening files
 * - read from file into an buffer
 *
 *  Copyright 2013 C3MA. All rights reserved.
 */

#ifndef HWAL_H
#define HWAL_H

#ifdef PRINT_DEBUG
#define DEBUG_PLINE( ... )	hwal_debug(__FILE__, __LINE__, __VA_ARGS__);
#else
#define DEBUG_PLINE( ... )	;
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#ifndef NULL	
#define NULL 				0 	/**< Pointer default value; needed but was in removed <string.h> */
#endif	
	
	/**
     * @brief Open an file.
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
	 * @param[in]	length	value that should be written into each item
	 * @param[in]	length	amount of bytes that should be read
	 */
	extern void hwal_memset(void *buffer, int item, int length);
	
	/**
	 * The memcpy() function copies n bytes from memory area s2 to memory area s1.
	 * If s1 and s2 overlap, behavior is undefined.  Applications in which s1 and s2
     * might overlap should use memmove(3) instead.
	 * 
	 * @param[out]	s1	to be filled
	 * @param[in]	s2	memory area with the information to read
	 * @param[in]	length	amount of bytes to be copied
	 */
	extern void hwal_memcpy(void *s1, void* s2, int length);
	
	/**
	 * Calculate the length of the given string
	 * @param[in]	text	the length should be calculated from
	 * @return calculated length for the given text
	 */
	extern int hwal_strlen(char* text);
	
	/**
	 * Interface for printing debugging messages IF DEBUG is enabled
	 * @param[in]	codefile	the file, where the message comes frome
	 * @param[in]	linenumber	the excat line of code of this code
	 * @param[in]	text		to display
	 */
	extern void hwal_debug(char* codefile, int linenumber, char* text, ...);
	
	/**
	 * Fetch some new memory
	 * @param[in]	size	in byte of the new memory to allocate
	 */
	extern void* hwal_malloc(int size);

	/**
	 * Give the memory of the given pointer back into the memory pool.	
	 * @param[in]	memory the memory, that should be released
	 */
	extern void hwal_free(void* memory);
	
	/**
	 * Create a new TCP socket
	 * @param[in] port	The portnumber the server should listen on
	 * @param[in] maximumClients the maximum of paralell opened connections
	 * @return a new Socket or negative numbers, on problems
	 */
	extern int hwal_socket_tcp_new(int port, int maximumClients);
	
	/** @fn extern void hwal_socket_tcp_close(int socketfd)
	 *
	 * Close the given socket
	 *
	 * @param[in] socketfd	Socket to close
	 */
	extern void hwal_socket_tcp_close(int socketfd);
	
	/** @fn extern int hwal_socket_tcp_accet(int socketfd)
	 *
	 * Accept a new client on a existing TCP socket.
	 * This function must be NONE blocking
	 *
	 * @param[in] socketfd	Already opened server socket
	 * @return a socket of a new client 
	 * or a negative number, if no new client was found.
	 */
	extern int hwal_socket_tcp_accet(int socketfd);
	
	/** @fn extern int hwal_socket_tcp_read(int clientSocket, uint8_t* workingMem, uint32_t workingMemorySize)
	 *
	 * @brief Read from an open socket
	 * @param[in]	clientSocket	Already opened client socket to read from
	 * @param[in]	workingMem		The memory the result is stored in
	 * @param[in]	workingMemorySize	The Length of the allocated memory
	 * @return amount of read bytes, or a negative number on errors
	 */
	extern int hwal_socket_tcp_read(int clientSocket, uint8_t* workingMem, uint32_t workingMemorySize);
	
	/** @fn extern int hwal_socket_tcp_write(int clientSocket, uint8_t* data, uint32_t size)
	 *
	 * @brief Write into an open socket
	 * @param[in]	clientSocket	Already opened client socket to write to
	 * @param[in]	data	The memory, containing the information to send
	 * @param[in]	size	The amount of bytes to send.
	 * @return amount of read bytes, or a negative number on errors
	 */
	extern int hwal_socket_tcp_write(int clientSocket, uint8_t* data, uint32_t size);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#endif
