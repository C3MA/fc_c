/*
 *  fcclient.h
 *  FC_C
 *
 *  Created by ollo on 24.04.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

/** @struct FCCLIENT Definition of a connection
 *  @brief This structure containing all attributes of a connection to the wall
 *  @var FCCLIENT::width 
 *  Member 'width' contains the width of the wall
 *  @var FCCLIENT::height 
 *  Member 'height' contains the height of the wall
 *  @var FCCLIENT::fps 
 *  Member 'fps' contains the expected frames per second (that is ignored when using the network protocol)
 *
 *  @var FCCLIENT::connected 
 *  Member 'connected' status if the wall expect us to send something
 *
 *  @var FCCLIENT::sockfd 
 *  Member 'sockfd' the socket, that is used
 */
struct FCCLIENT {
	int width;
	int height;
	int fps;
	
	int connected;
	
	int sockfd;
};

typedef struct FCCLIENT fcclient_t;

/*
 * creation of a new connection
 * allocating new memory.
 * YOU have to free this memory after using!
 */
extern fcclient_t* fcclient_new();

/*
 * open a connection to the specific host
 * @param[in|out] fc actual using client
 * @param[in] host hostname
 * @return success (> 0)
 */
extern int fcclient_open(fcclient_t* fc, char* host);

/*
 * process the network functionality
 * This function has to be called cyclically to process incoming answers from the wall
 * @param[in|out] fc actual using client
 * @return success (> 0)
 */
extern int fcclient_processNetwork(fcclient_t* fc);

/*
 * Start the sending of new frames to the wall
 * @param[in|out] fc actual using client
 * @return success (> 0)
 */
extern int fcclient_start(fcclient_t* fc);
	
	
/*
 * process the network functionality
 * This function has to be called cyclically to process incoming answers from the wall
 * @param[in|out] fc actual using client
 * @return success (> 0)
 */
extern int fcclient_sendFrame(fcclient_t* fc, int* buffer);
	
	
#if 0
	/***TODO some parked interfaces, that would be probalby useful **/
	
	/**
	 * entry point to prepare the cyclically sending of frames (all needed objects are allocated here)
	 * This mechanism reduces the GC calls and should increase the performance
	 */
	extern int fcclient_generateFrame(fcclient_t* fc);
	extern int fcclient_updatePixel(int red, int green, int blue, int x, int y);
#endif
	
	
#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif