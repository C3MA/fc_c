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
#include <string.h>

/*
 * @param[in] pInput buffer with the protobuf content
 * @param[out] pOutput the buffer to fill with input and the corresponding header
 * @param[in] the length of the input protobuf data
 */
void add_header(uint8_t* pInput, uint8_t* pOutput, int lengthInput)
{
    char header[12];
    sprintf(header, "%10d", lengthInput);
    memcpy(pOutput, header, 10);
    memcpy(pOutput+10, pInput, lengthInput);
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] typ of the snip
 * @return the new offset
 */
int add_type(uint8_t *buffer, int offset, int typ)
{
    buffer[offset] = serialize(SNIP_TYPE, PROTOTYPE_VARIANT);
    offset++;
    offset = serialize_number(buffer, offset, typ);
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] sniptyp of the snip
 * @param[out] length of the snip 
 * @return the new offset
 */
int get_header(uint8_t *buffer, int offset, int *sniptyp, int *length)
{
    int id, type;
    if (sscanf((char*)buffer, "%10d",length)!= 1) {
        return -1;
    }
    offset += 10;
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_TYPE || type != PROTOTYPE_VARIANT)
        return -1;
    
    offset = parse_number(buffer, offset, sniptyp);
    
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] value the counter, that was read
 * @return the new offset
 */
int recv_ping(uint8_t *buffer, int offset, int *value)
{
    int id, type;
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_PINGSNIP || type != PROTOTYPE_LENGTHD)
        return -1;
    
    offset = parse_number(buffer, offset, &type); // use type to store the length
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
    
    offset = add_type(buffer, offset, SNIPTYPE_PING);
    
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
    
    offset = add_type(buffer, offset, SNIPTYPE_PONG);
    
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
