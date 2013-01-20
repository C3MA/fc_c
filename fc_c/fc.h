//
//  fc.h
//  fc_c
//
//  Created by Christian Platz on 19.01.13.
//  Copyright (c) 2013 C3MA. All rights reserved.
//

#ifndef fc_c_fc_h
#define fc_c_fc_h

#include <stdint.h>

#define SNIP_PINGSNIP   11
#define SNIPTYPE_PING   1
#define PINGSNIP_COUNT 1

#define SNIP_PONGSNIP   12
#define SNIPTYPE_PONG   2
#define PONGSNIP_COUNT 1

#define NETWORK_PORT 24567

int recv_ping(uint8_t *buffer, int offset, int *value);
int send_ping(uint8_t *buffer, int offset, int counter);
int recv_pong(uint8_t *buffer, int offset, int *value);
int send_pong(uint8_t *buffer, int offset, int counter);

#endif
