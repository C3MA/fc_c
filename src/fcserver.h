/* @file fcserver.h
 * @brief Server implementation
 * @author Ollo
 *
 * @date 16.10.2013 – New Server API with some ideas
 * @defgroup Server
 * 
 * Copyright 2013 C3MA. All rights reserved.
 */

#ifndef FC_C_SERVER_H
#define FC_C_SERVER_H


/** @addtogroup Server */
/*@{*/

#include <stdint.h>
#include "hwal.h"

#define	FCSERVER_MAXCLIENT	5

#define FCSERVER_TEMPMEM_MAX	2048	/**< Maximum size of the temporary memory */

#define FCSERVER_DEFAULT_FPS		60	/**< Default value for the frames per seconds */
#define FCSERVER_DEFAULT_NAME		"fc-c Server"
#define FCSERVER_DEFAULT_VERSION	"0.1.1.0"

/* Define possible error codes, that can be returned to the client */
#define FCSERVER_ERR_RESOLUTION		10

/** @enum FCSERVER_RET
 * @typedef fcserver_ret_t
 * @brief Status code, that is used in this module
 * A summary of all possible return states in this module
 */
enum FCSERVER_RET 
{
	FCSERVER_RET_OK=1, /**< Function did not detect any problems  */
	FCSERVER_RET_NODATA, /**< No new data was found on the socket  */
	FCSERVER_RET_IOERR, /**< Input/Output error */
	FCSERVER_RET_EOF, /**< file end reached */
	FCSERVER_RET_PARAMERR, /**< invalid parameter were used (NULL pointer) */
	FCSERVER_RET_NOTIMPL, /**< not implementend atm */
	FCSERVER_RET_INVALID_DATA, /**< Found invalid data */
	FCSERVER_RET_OUTOFMEMORY /**< Give US MORE memory! */
};
typedef enum FCSERVER_RET fcserver_ret_t;

/** @fn (*ImageCallback_t)
 * Action, that should be performed on a new received image.
 *
 * @param[in] rgb24Buffer	Memory buffer containing the actual image.
 * @param[in] width			in pixel of the actual frame
 * @param[in] height		in pixel of the actual frame
 *
 * @return NOTHING
 */
typedef void (*ImageCallback_t) (uint8_t* rgb24Buffer, int width, int height);

/** @var FCSERVER
 *  @var fcserver_t
 *  @brief This structure contains meta information of the actual server instance
 *  @var FCSERVER::clientcount 
 *  Member 'clientcount' has the status information about the actual connected clients
 *  
 *  @var FCSERVER::type 
 *  Member 'type' defines the source
 */
struct FCSERVER {
	int						width;/**< Horizontal count of boxes the phyical installation */
	int						height;/**< Vertical count of boxes the phyical installation */
	uint8_t*				tmpMem; /**< Pointer to already allocated memory to work with */
	uint32_t				tmpMemSize;	/**< Length of th allocated memory, stored in tmpMem */
	uint8_t					clientcount; /**< information about the actual connected clients */
	int						serversocket; /**< The socket for the server */
	int	clientsocket[FCSERVER_MAXCLIENT]; /**< Space for up to @see FCSERVER_MAXCLIENT clients */
	ImageCallback_t			onNewImage; /**< Reference to logic, when a new image should be displayed */
};
typedef struct FCSERVER fcserver_t;	/**< has the status information about the actual connected clients */

/** @fn fcserver_ret_t fcserver_init (fcserver_t* server, ImageCallback_t onNewImage, ClientCallback_t onNewClient)
 * @brief Initialize the library
 * @param[in,out]	server	structure, representing the actual status of the server (must be already allocated)
 * @param[in]		onNewImage	provide an callback of type "ImageCallback_t" to define the action on a received frame
 * @param[in]		width		of the phyical installation
 * @param[in]		height		of the phyical installation
 *
 * @return status 
 * - FCSERVER_RET_OK ( that what we expect)
 * - FCSERVER_RET_OUTOFMEMORY (you have it! give more memory)
 * - FCSERVER_RET_PARAMERR You forgot an important parameter
 */
fcserver_ret_t fcserver_init (fcserver_t* server, ImageCallback_t onNewImage,
							  int width, int height);

/** @fn fcserver_ret_t fcserver_process (fcserver_t* server)
 * @brief Initialize the library
 * @param[in]	server	structure, representing the actual status of the server
 *
 * @return status 
 */
fcserver_ret_t fcserver_process (fcserver_t* server);

fcserver_ret_t fcserver_close(fcserver_t* server);

/*@}*/

#endif