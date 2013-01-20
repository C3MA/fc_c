//
//  fc.c
//  fc_c
//
//  Created by Christian Platz on 19.01.13.
//  Copyright (c) 2013 C3MA. All rights reserved.
//

#include <stdio.h>
#include "proto.h"
#include "fc.h"

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] value the counter, that was read
 * @return the new offset
 */
int recv_ping(uint8_t *buffer, int offset, int *value)
{
    /*
     send msg snip, typ ping, mesg ping mit count
     */
    int id, type;
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_PINGSNIP || type != PROTOTYPE_LENGTHD)
        return -1;
    
    offset = parse_number(buffer, offset, &type); // use type to store the length
    printf("Offset in rcv_ping = %d, length of dataset=%d\n",offset, type);
    offset = parse(buffer, offset, &id, &type);
    if (id != PINGSNIP_COUNT || type != PROTOTYPE_VARIANT)
    {
        (*value) = -1;
        return offset;
    }
    else
    {
        int tmpOffset;
        tmpOffset = parse_number(buffer,offset, value);
        return tmpOffset;
    }
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] value the counter, to write
 * @return the new offset
 */
int send_ping(uint8_t *buffer, int offset, int counter)
{
    int offset4length;
    
    /*
     send msg snip, typ ping, mesg ping mit count
     */
    
    /*
     * Write the header for Ping structure
     */
    buffer[offset] = serialize(SNIP_PINGSNIP, PROTOTYPE_LENGTHD);
    offset++;
    // offset +1 : here the length must be caluculated
    offset4length = offset;
    offset++;
    
    /*
     * store the value into the buffer
     */
    buffer[offset] = serialize(PINGSNIP_COUNT, PROTOTYPE_VARIANT);
    offset++;
    offset = serialize_number(buffer, offset, counter);
    buffer[offset4length] = offset - offset4length - 1;
    
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] value the counter, that was read
 * @return the new offset
 */
int recv_pong(uint8_t *buffer, int offset, int *value)
{
    /*
     send msg snip, typ ping, mesg ping mit count
     */
    int id, type;
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_PONGSNIP || type != PROTOTYPE_LENGTHD)
        return -1;
    
    offset = parse_number(buffer, offset, &type); // use type to store the length
    printf("Offset in rcv_pong = %d, length of dataset=%d\n",offset, type);
    offset = parse(buffer, offset, &id, &type);
    if (id != PONGSNIP_COUNT || type != PROTOTYPE_VARIANT)
    {
        (*value) = -1;
        return offset;
    }
    else
    {
        int tmpOffset;
        tmpOffset = parse_number(buffer,offset, value);
        return tmpOffset;
    }
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] value the counter, to write
 * @return the new offset
 */
int send_pong(uint8_t *buffer, int offset, int counter)
{
    int offset4length;
    
    /*
     send msg snip, typ ping, mesg ping mit count
     */
    
    /*
     * Write the header for Ping structure
     */
    buffer[offset] = serialize(SNIP_PONGSNIP, PROTOTYPE_LENGTHD);
    offset++;
    // offset +1 : here the length must be caluculated
    offset4length = offset;
    offset++;
    
    /*
     * store the value into the buffer
     */
    buffer[offset] = serialize(PONGSNIP_COUNT, PROTOTYPE_VARIANT);
    offset++;
    offset = serialize_number(buffer, offset, counter);
    buffer[offset4length] = offset - offset4length - 1;
    
    return offset;
}
