//
//  proto.c
//  fc_c
//

#include "proto.h"
#include <math.h>

/**
 * @param[in] buffer	Memory to read
 * @param[in] offset	where to start in the buffer
 * @param[out] pId		the found id at the position in the buffer
 * @param[out] pType	the found type at the position in the buffer
 * @return the new offset or "-1" on problems
 *
 * Reads the ID and the type from the given memory at the specified offset.
 */
extern int parse(uint8_t *buffer, int offset, int* pId, int* pType)
{
    int in;
    offset = parse_number(buffer, offset, &in);
    // Error
    if (offset == -1) {
        return -1;
    }
    *pType = (in & 0x7);
    *pId = in>>3;
    return offset;
}

/**
 * @param[in] buffer	the memory area to write into
 * @param[in] offset	position in the memory were to write
 * @param[in] id		that should be written
 * @param[in] type		that should be written
 * @return new position or "-1" on problems
 * 
 * Write an ID and the type (see both defines) into the memory
 */
extern int serialize(uint8_t *buffer, int offset, int id, int type)
{
    int out;
    // Error
    if (offset == -1) {
        return -1;
    }
    out = type & 0x7;
    out = out | (id << 3);
    offset = serialize_number(buffer, offset, out);
    return offset;
}

/**
 * @param[in] buffer	memory containing the protobuf structure
 * @param[in] offset	actual position on the buffer
 * @param[out] value	number, that was read
 * @return the new offset or "-1" on problems
 *
 * Reads an integer from the memory
 */
extern int parse_number(uint8_t *buffer, int offset, int *value)
{
    // Error
    if (offset == -1) {
        return -1;
    }
    int count = -1;
    *value=0;
    // check if the first bit is set (so the following byte must be read)
    do {
        count++;
        (*value) += pow(2, 7*count) * (buffer[offset+count] & 0x7F);
    } while(buffer[offset+count] & 0x80);
    
    return count+1+offset;
}

/**
 * @param[in] buffer	memory with space for the structure
 * @param[in] offset	actual position, where the data was read
 * @param[out] value	number, that will be written
 * @return the new offset or "-1" on problems
 *
 * Writes a number into the memory
 * @TODO was passiert bei value 0?
 */
extern int serialize_number(uint8_t *buffer, int offset, int value)
{
    // Error
    if (offset == -1) {
        return -1;
    }
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