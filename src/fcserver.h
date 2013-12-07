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

#define FCSERVER_DEFAULT_FPS		24	/**< Default value for the frames per seconds */
#define FCSERVER_DEFAULT_NAME		"fc-c Server"
#define FCSERVER_DEFAULT_VERSION	"0.1.1.0"

/* Define possible error codes, that can be returned to the client */
#define FCSERVER_ERR_RESOLUTION		10
#define FCSERVER_ERR_MAXCLIENTS		11
#define	FCSERVER_ERR_INTERRUPTED	12 /**< Interrupted by an file */
#define	FCSERVER_ERR_TOO_SLOW		13 /**< Client must send its frames faster! */


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
	FCSERVER_RET_OUTOFMEMORY, /**< Give US MORE memory! */
	FCSERVER_RET_CLOSED, /**< Socket was closed! */
	FCSERVER_RET_NOTHINGNEW	/**< No new packet was received */
};
typedef enum FCSERVER_RET fcserver_ret_t;

/** @enum FCSERVER_RET
 * @typedef fcserver_ret_t
 * @brief Status code, that is used in this module
 * A summary of all possible return states in this module
 */
enum FCCLIENT_STATUS 
{
	FCCLIENT_STATUS_WAITING=1, /**< The client is waiting for a GO from the server  */
	FCCLIENT_STATUS_CONNECTED, /**<  The clients is sending pictures to the wall */
	FCCLIENT_STATUS_INITING, /**<  First contact of the client with the server */
	FCCLIENT_STATUS_DISCONNECTED, /**<  The clients has left */
	FCCLIENT_STATUS_TOOMUTCH /**<  There is no space to handle also this client */
};
typedef enum FCCLIENT_STATUS	fclientstatus_t;

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

/** @fn (*ClientCallback_t)
 * Action, that should be performed on a new received image.
 *
 * @param[in] totalAmount	The amount of action connected clients
 * @param[in] width			in pixel of the actual frame
 * @param[in] height		in pixel of the actual frame
 *
 * @return NOTHING
 */
typedef void (*ClientCallback_t) (uint8_t totalAmount, fclientstatus_t action, int clientsocket);

/** @var FCCLIENT
 *  @var fcclient_t
 *  @brief This structure stores the status of one connected client.
 *  @var FCCLIENT::clientsocket
 *  Member 'clientsocket' stores the socket id for the connection to the client.
 *  @var FCCLIENT::clientstatus
 *  Member 'type' defines the source
 */
struct FCCLIENT {
	int				clientsocket;
	fclientstatus_t clientstatus;
};
typedef struct FCCLIENT fcclient_t;	/**< has the status information about the actual connected clients */

/** @var FCSERVER
 *  @var fcserver_t
 *  @brief This structure contains meta information of the actual server instance
 *  @var FCSERVER::clientcount 
 *  Member 'clientcount' has the status information about the actual connected clients
 *  @var FCSERVER::type 
 *  Member 'type' defines the source
 */
struct FCSERVER {
	int						status; /**< Actual status of the server, (active/deactivated) */
	int						width;/**< Horizontal count of boxes the phyical installation */
	int						height;/**< Vertical count of boxes the phyical installation */
	uint8_t*				imageBuffer; /**< Buffer to store a received frame */
	uint8_t*				tmpMem; /**< Pointer to already allocated memory to work with */
	uint32_t				tmpMemSize;	/**< Length of th allocated memory, stored in tmpMem */
	uint32_t				reading_offset; /**< Actual buffer length, necessary, if informations are fragmented on different packets */
	uint8_t					clientamount; /**< information about the number of actual connected clients */
	int						serversocket; /**< The socket for the server */
	fcclient_t	client[FCSERVER_MAXCLIENT]; /**< Space for up to @see FCSERVER_MAXCLIENT clients */
	uint32_t				receivedLevelMs;	/**< This level is increased each cycle, (by the elapsed milliseconds)
												 a new frame pulls it up, this trigger must never get empty */
	
	ImageCallback_t			onNewImage; /**< Reference to logic, when a new image should be displayed */
	ClientCallback_t		onClientChange; /**< Reference to logic, when client joins or leave */
};
typedef struct FCSERVER fcserver_t;	/**< has the status information about the actual connected clients */

/** @fn fcserver_ret_t fcserver_init (fcserver_t* server, ImageCallback_t onNewImage, ClientCallback_t onNewClient)
 * @brief Initialize the library
 * @param[in,out]	server	structure, representing the actual status of the server (must be already allocated)
 * @param[in]		onNewImage	provide an callback of type "ImageCallback_t" to define the action on a received frame
 * @param[in]		onClientChange	provide an callback of type "ClientCallback_t" to define the action on change on the incoming clients
 * @param[in]		width		of the phyical installation
 * @param[in]		height		of the phyical installation
 *
 * @return status 
 * - FCSERVER_RET_OK ( that what we expect)
 * - FCSERVER_RET_OUTOFMEMORY (you have it! give more memory)
 * - FCSERVER_RET_PARAMERR You forgot an important parameter
 */
fcserver_ret_t fcserver_init (fcserver_t* server, ImageCallback_t onNewImage, ClientCallback_t	onClientChange,
							  int width, int height);

/** @fn fcserver_ret_t fcserver_setactive (fcserver_t* server, int status)
 * @brief Set the server to active
 * @param[in,out]	server	structure, representing the actual status of the server (must be already allocated)
 * @param[in]		status	if 0, the server does not allow new clients to send something; on status = 1 dynamic content is processed
 *
 * @return status 
 * - FCSERVER_RET_OK ( that what we expect)
 * - FCSERVER_RET_PARAMERR You forgot an important parameter
 */
fcserver_ret_t fcserver_setactive (fcserver_t* server, int status);

/** @fn fcserver_ret_t fcserver_process (fcserver_t* server)
 * @brief Initialize the library
 * @param[in]	server		structure, representing the actual status of the server
 * @param[in]	cycletime	duration in milliseconds this function is be called approximately again.
 *
 * @return status 
 */
fcserver_ret_t fcserver_process (fcserver_t* server, int cycletime);

fcserver_ret_t fcserver_close(fcserver_t* server);

/*@}*/

#endif
