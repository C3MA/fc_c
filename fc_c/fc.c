//
//  fc.c
//  fc_c
//
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
    char header[HEADER_LENGTH+2];
    sprintf(header, "%10d", lengthInput);
    memcpy(pOutput, header, HEADER_LENGTH);
    memcpy(pOutput+HEADER_LENGTH, pInput, lengthInput);
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
    offset += HEADER_LENGTH;
    offset = parse(buffer, offset, &id, &type);
    if (id != SNIP_TYPE || type != PROTOTYPE_VARIANT)
        return -1;
    
    offset = parse_number(buffer, offset, sniptyp);
    
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[in] id of the variant
 * @param[in] value
 * @return the new offset
 */
// TODO: find better Name for ID!
int add_variant(uint8_t *buffer, int offset, int id ,int value)
{
    buffer[offset] = serialize(id, PROTOTYPE_VARIANT);
    offset++;
    offset = serialize_number(buffer, offset, value);
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[in] typ of the snip
 * @return the new offset
 */
//TODO: Check if necessary
int add_type(uint8_t *buffer, int offset, int typ)
{
    add_variant(buffer, offset, SNIP_TYPE, typ);
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
    offset = add_variant(buffer, offset, PINGSNIP_COUNT, counter);
    //TODO: check if serialize_number?
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
 * @param[in] value the counter, to write
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
    offset = add_variant(buffer, offset, PONGSNIP_COUNT, counter);
    //TODO: check if serialize_number?
    buffer[offset4length] = offset - offset4length - 1;
    
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[in] color, string with color
 * @param[in] seqId, ID of the Sequence
 * @param[in] meta, buffer with Binarysequenzemetadta
 * @return the new offset
 */
int send_request(uint8_t *buffer, int offset, char *color, int seqId, uint8_t *meta, int offset_meta)
{
    
}


/*
 * @param[in] buffer
 * @param[in] offset
 * @param[in] frames_per_second
 * @param[in] width
 * @param[in] height
 * @param[in] generator_name
 * @param[in] generator_version 
 * @return the new offset
 */
// TODO: sizeof ist nicht die Lösung da muss was her was die Bytes bis 0 Zählt!
int create_metadata(uint8_t *buffer, int offset, uint32_t frames_per_second, uint32_t width, uint32_t heigtht, char *generator_name, char *generator_version)
{
    long length_generator_name, length_generator_version;
    
    offset = add_variant(buffer, offset, BINARYSEQUENCEMETADATA_FRAMESPERSECOND, frames_per_second);
    
    offset = add_variant(buffer, offset, BINARYSEQUENCEMETADATA_WIDTH, width);
    offset = add_variant(buffer, offset, BINARYSEQUENCEMETADATA_HEIGHT, heigtht);
    
    length_generator_name = sizeof(generator_name)-3; //BOESE
    buffer[offset] = serialize(BINARYSEQUENCEMETADATA_GENERATORNAME, PROTOTYPE_LENGTHD);
    offset++;
    offset = serialize_number(buffer, offset, (int) length_generator_name);
    memcpy(buffer+offset, generator_name, length_generator_name);
    offset +=  (int) length_generator_name;
    
    length_generator_version = sizeof(generator_version)-4;//BOESE
    buffer[offset] = serialize(BINARYSEQUENCEMETADATA_GENERATORVERSION, PROTOTYPE_LENGTHD);
    offset++;
    offset = serialize_number(buffer, offset, (int) length_generator_version);
    memcpy(buffer+offset, generator_version, length_generator_version);
    offset +=  (int) length_generator_version;
    
    return offset;
}





