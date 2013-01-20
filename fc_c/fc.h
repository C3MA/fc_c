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

#define SNIP_TYPE 1

#define SNIPTYPE_PING 1
#define SNIPTYPE_PONG 2
#define SNIPTYPE_ERROR 3
#define SNIPTYPE_REQUEST 4
#define SNIPTYPE_START 5
#define SNIPTYPE_FRAME 6
#define SNIPTYPE_ACK 7
#define SNIPTYPE_NACK 8
#define SNIPTYPE_TIMEOUT 9
#define SNIPTYPE_ABORT 10
#define SNIPTYPE_EOS 11

#define SNIP_PINGSNIP   11
#define PINGSNIP_COUNT 1

#define SNIP_PONGSNIP   12
#define PONGSNIP_COUNT 1

#define HEADER_LENGTH 10

#define NETWORK_PORT 24567

void add_header(uint8_t* pInput, uint8_t* pOutput, int lengthInput);
int get_header(uint8_t *buffer, int offset, int *sniptyp, int *length);
int recv_ping(uint8_t *buffer, int offset, int *value);
int send_ping(uint8_t *buffer, int offset, int counter);
int recv_pong(uint8_t *buffer, int offset, int *value);
int send_pong(uint8_t *buffer, int offset, int counter);

#endif
