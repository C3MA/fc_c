//
//  proto.c
//  fc_c
//
//  Created by Christian Platz on 19.01.13.
//  Copyright (c) 2013 C3MA. All rights reserved.
//

#include "proto.h"
#include <math.h>

/**
 * @param[in] buffer
 * @param[in] offset where to start in the buffer
 * @param[out] pId the found id at the position in the buffer
 * @param[out] pType the found type at the position in the buffer
 * @return the new offset
 */
int parse(uint8_t *buffer, int offset, int* pId, int* pType)
{
    *pType = (buffer[offset] & 0x7);
    *pId = buffer[offset]>>3;
    return offset+1;
}

/*
 * @param[in] id
 * @param[in] type
 * @return combined byte with both values
 */
uint8_t serialize(int id, int type)
{
    uint8_t out;
    out = type & 0x7;
    out = out | (id << 3);
    return out;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] value that was read
 * @return the new offset
 */
int parse_number(uint8_t *buffer, int offset, int *value)
{
    int count = -1;
    *value=0;
    // check if the first bit is set (so the following byte must be read)
    do {
        count++;
        (*value) += pow(2, 7*count) * (buffer[offset+count] & 0x7F);
    } while(buffer[offset+count] & 0x80);
    
    return count+1+offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] value that will be written
 * @return the new offset
 */
int serialize_number(uint8_t *buffer, int offset, int value)
{
    do {
        buffer[offset] = value & 0x7F;
        value = value>>7;
        // always set the first bit to indicate more bytes
        buffer[offset] |= 0x80;
        offset++;
    } while (value > 0);
    
    // last byte ! -> correct the first bit
    buffer[offset - 1] = buffer[offset - 1] & 0x7F;
    
    return offset;
}