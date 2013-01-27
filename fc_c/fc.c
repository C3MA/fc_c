//
//  fc.c
//  fc_c
//
//

#include <stdio.h>
#include <stdlib.h>
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
 * @param[in] proto_id of the variant
 * @param[in] value
 * @return the new offset
 */
int add_variant(uint8_t *buffer, int offset, int proto_id ,int value)
{
    offset = serialize(buffer, offset, proto_id, PROTOTYPE_VARIANT);
    offset = serialize_number(buffer, offset, value);
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[in] typ of the snip
 * @return the new offset
 */
int add_type(uint8_t *buffer, int offset, int typ)
{
    offset = add_variant(buffer, offset, SNIP_TYPE, typ);
    return offset;
}


/*
 * @param[in|out] buffer
 * @param[in] offset
 * @param[in] proto_id
 * @param[in] data buffer
 * @param[in] legngth data
 * @return the new offset
 */
int add_lengthd(uint8_t *buffer, int offset, int proto_id ,uint8_t *data, long length)
{
    offset = serialize(buffer, offset, proto_id, PROTOTYPE_LENGTHD);
    offset = serialize_number(buffer, offset, (int) length);
    memcpy(buffer+offset, data, length);
    offset +=  (int) length;
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[in] proto_id
 * @return the new offset
 */
int add_lengthd_empty(uint8_t *buffer, int offset, int proto_id)
{
    offset = serialize(buffer, offset, proto_id, PROTOTYPE_LENGTHD);
    buffer[offset] = 0x00;
    offset++;
    return offset;
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
    offset = serialize(buffer, offset, SNIP_PINGSNIP, PROTOTYPE_LENGTHD);
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
    offset = serialize(buffer, offset, SNIP_PONGSNIP, PROTOTYPE_LENGTHD);
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
 * @param[out] value the counter, that was read
 * @return the new offset
 */
int recv_pong(uint8_t *buffer, int offset, int *value)
{
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
 * @param[in] color, string with color
 * @param[in] seqId, ID of the Sequence
 * @param[in] meta, buffer with Binarysequenzemetadta
 * @return the new offset
 */
// TODO: TEST
int send_request(uint8_t *buffer, int offset, char *color, int seqId, uint8_t *meta, int length_meta)
{
    offset = add_lengthd(buffer, offset, REQUESTSNIP_COLOR, (uint8_t*) color, strlen(color));
    
    offset = add_variant(buffer, offset, REQUESTSNIP_SEQID, seqId);
    
    offset = add_lengthd(buffer, offset, REQUESTSNIP_META, meta, length_meta);
    
    return offset;
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
// TODO: TEST
int create_metadata(uint8_t *buffer, int offset, int frames_per_second, int width, int heigtht, char *generator_name, char *generator_version)
{    
    offset = add_variant(buffer, offset, BINARYSEQUENCEMETADATA_FRAMESPERSECOND, frames_per_second);
    
    offset = add_variant(buffer, offset, BINARYSEQUENCEMETADATA_WIDTH, width);
    offset = add_variant(buffer, offset, BINARYSEQUENCEMETADATA_HEIGHT, heigtht);
    
    offset = add_lengthd(buffer, offset, BINARYSEQUENCEMETADATA_GENERATORNAME, (uint8_t*) generator_name, strlen(generator_name));
    
    offset = add_lengthd(buffer, offset, BINARYSEQUENCEMETADATA_GENERATORVERSION, (uint8_t*) generator_version, strlen(generator_version));
    
    return offset;
}

/*
 
 */

int parse_metadata(uint8_t *meta, int meta_length, )
{
    
    
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @param[out] color, pointer to memory area of color [YOU have to FREE this Memory later!1!]
 * @param[out] seqId
 * @param[out] meta, pointer of memory area of Metadata [YOU have to FREE this Memory later!1!]
 * @param[out] meta_length, length of the Metadata
 * @return the new offset
 */
int recv_request(uint8_t *buffer, int offset, char **color, int *seqId, uint8_t **meta, int *meta_length)
{
    int id, type, length;
    offset = parse(buffer, offset, &id, &type); // Read first byte and check if Right snip
    if (id != SNIP_REQUESTSNIP || type != PROTOTYPE_LENGTHD)
        return -1;
    
    offset = parse_number(buffer, offset, &length); // Read length of req_snip
    
     // Read Color
    
    offset = parse(buffer, offset, &id, &type);
    if (id != REQUESTSNIP_COLOR || type != PROTOTYPE_LENGTHD)
    {
        offset = parse_number(buffer, offset, &length);
        *color = (char*) malloc((long) length+1);   // +1 for 0x00 (string end)
        memcpy(*color, buffer+offset, (long) length);
        (*color)[length] = 0x00; // string ende
        offset += length;
    }
    else
    {
        return -1;
    }
    
    // Read SeqID
    
    offset = parse(buffer, offset, &id, &type); // Read first byte and check if Right snip
    if (id != REQUESTSNIP_SEQID || type != PROTOTYPE_VARIANT)
        return -1;
    offset = parse_number(buffer, offset, seqId); // Read value of SeqId
    
    // Read Metadata
    
    offset = parse(buffer, offset, &id, &type);  // Read Color
    if (id != REQUESTSNIP_META || type != PROTOTYPE_LENGTHD)
    {
        offset = parse_number(buffer, offset, meta_length);
        *meta = (uint8_t*) malloc((long) meta_length);
        memcpy(*meta, buffer+offset, (long) meta_length);
        offset += *meta_length;
    }
    else
    {
        return -1;
    }

    return offset;
}


/*
 * @param[in] buffer
 * @param[in] offset
 * @return the new offset
 */
// TODO: TEST
int send_start(uint8_t *buffer, int offset)
{
    offset = add_type(buffer, offset, SNIPTYPE_START);
    offset = add_lengthd_empty(buffer, offset, SNIP_STARTSNIP);
    return offset;
}

/*
 * @param[in|out] buffer
 * @param[in] offset
 * @param[in] red
 * @param[in] green
 * @param[in] blue
 * @param[in] x
 * @param[in] y
 * @return the new offset
 */
int frame_add_pixel(uint8_t *buffer, int offset, int red, int green, int blue, int x, int y)
{
    int offset4length;
    
    /*
     * Write the header for pixel structure
     */
    offset = serialize(buffer, offset, BINARYFRAME_PIXEL, PROTOTYPE_LENGTHD);
    // offset +1 : here the length must be caluculated
    offset4length = offset;
    offset++;
    
    /*
     * store the value into the buffer
     */
    offset = add_variant(buffer, offset, RGBVALUE_RED, red);
    offset = add_variant(buffer, offset, RGBVALUE_GREEN, green);
    offset = add_variant(buffer, offset, RGBVALUE_BLUE, blue);
    offset = add_variant(buffer, offset, RGBVALUE_X, x);
    offset = add_variant(buffer, offset, RGBVALUE_Y, y);
    //TODO: check if serialize_number?
    buffer[offset4length] = offset - offset4length - 1;
    return offset;
}

/*
 * @param[in|out] buffer
 * @param[in] offset
 * @param[in] frames buffer with frame
 * @param[in] length_frames length of buffer
 * @return the new offset
 */
int send_frame(uint8_t *buffer, int offset, uint8_t *frame, long length_frame)
{
    int lenght_frame_length;
    uint8_t length_frame_serialized[10]; //TODO: Größe ermitteln
    
    offset = add_type(buffer, offset, SNIPTYPE_FRAME);
    
    lenght_frame_length = serialize_number(length_frame_serialized, 0, (int)length_frame);
    
    offset = serialize(buffer, offset, SNIP_FRAMESNIP, PROTOTYPE_LENGTHD);
    buffer[offset] = 0x01;
    offset++;
    /*
     * Add header for Frames, with two lenght values. Calculate first with length + length of next header :-/
     */
    offset = serialize_number(buffer, offset, (int)length_frame + lenght_frame_length + 1);
    
    offset = add_lengthd(buffer, offset, FRAMESNIP_FRAME, frame, (long)length_frame);
    
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @return the new offset
 */
int send_ack(uint8_t *buffer, int offset)
{
    offset = add_type(buffer, offset, SNIPTYPE_ACK);
    offset = add_lengthd_empty(buffer, offset, SNIP_ACKSNIP);
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @return the new offset
 */
int send_nack(uint8_t *buffer, int offset)
{
    offset = add_type(buffer, offset, SNIPTYPE_NACK);
    offset = add_lengthd_empty(buffer, offset, SNIP_NACKSNIP);
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @return the new offset
 */
int send_timeout(uint8_t *buffer, int offset)
{
    offset = add_type(buffer, offset, SNIPTYPE_TIMEOUT);
    offset = add_lengthd_empty(buffer, offset, SNIP_TIMEOUTSNIP);
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @return the new offset
 */
int send_abort(uint8_t *buffer, int offset)
{
    offset = add_type(buffer, offset, SNIPTYPE_ABORT);
    offset = add_lengthd_empty(buffer, offset, SNIP_ABORTSNIP);
    return offset;
}

/*
 * @param[in] buffer
 * @param[in] offset
 * @return the new offset
 */
int send_eos(uint8_t *buffer, int offset)
{
    offset = add_type(buffer, offset, SNIPTYPE_EOS);
    offset = add_lengthd_empty(buffer, offset, SNIP_EOSSNIP);
    return offset;
}
