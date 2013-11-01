#ifndef HWAL_NET_H
#define HWAL_NET_H

#include "ch.h"
#include "hwal.h"
#include "ff.h"

#ifndef HWAL_NET_THREAD_STACK_SIZE
#define HWAL_NET_THREAD_STACK_SIZE   THD_WA_SIZE(512)
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Create a new TCP socket and handle the accept functionality in a seperate thread
 * 
 * @param[in] port	The portnumber the server should listen on
 * @param[in] maximumClients the maximum of paralell opened connections
 * @return a new Socket or negative numbers, on problems
 */
int	hwalnet_new_socket(int port, int maximumClients);

/**
 * Checks the seperate thread for new clients, that want to be handled.
 *
 * @param[in] socketConnection	the server socket, that should be checked
 * @return newClientsocket or -1 if no new socket was found
 */
int hwalnet_new_client( int socketConnection );
	
#ifdef __cplusplus
}
#endif

#endif /* End of HWAL_NET_H */
