//
//  proto.h
//  fc_c
//
//  Created by Christian Platz on 19.01.13.
//  Copyright (c) 2013 C3MA. All rights reserved.
//

#ifndef fc_c_proto_h
#define fc_c_proto_h

#include <stdint.h>

#define PROTOTYPE_VARIANT   0
#define PROTOTYPE_LENGTHD   2


int parse(uint8_t *buffer, int offset, int* pId, int* pType);
uint8_t serialize(int id, int type);
int parse_number(uint8_t *buffer, int offset, int *value);
int serialize_number(uint8_t *buffer, int offset, int value);

#endif
