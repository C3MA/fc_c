//
//  fc.h
//  fc_c
//
//

#ifndef fc_c_fc_h
#define fc_c_fc_h

#include <stdint.h>

// Defines based on sequence.proto from Fullcircle
// BinarySequenceMetadata
#define BINARYSEQUENCEMETADATA_FRAMESPERSECOND 1
#define BINARYSEQUENCEMETADATA_WIDTH 2
#define BINARYSEQUENCEMETADATA_HEIGHT 3
#define BINARYSEQUENCEMETADATA_GENERATORNAME 4
#define BINARYSEQUENCEMETADATA_GENERATORVERSION 5

// RGBValue
#define RGBVALUE_RED 1
#define RGBVALUE_GREEN 2
#define RGBVALUE_BLUE 3
#define RGBVALUE_X 4
#define RGBVALUE_Y 5

// BinaryFrame
#define BINARYFRAME_PIXEL 1

// BinarySequence
#define BINARYSEQUENCE_METADATA 1
#define BINARYSEQUENCE_FRAME 2

// Network Snip
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

#define ERRORCODETYPE_OK 1
#define ERRORCODETYPE_DECODING_ERROR 2

#define SNIP_ERRORSNIP 13
#define ERRORSNIP_ERRORCODE 1
#define ERRORSNIP_DESCRIPTION 2

#define SNIP_REQUESTSNIP 14
#define REQUESTSNIP_COLOR 1
#define REQUESTSNIP_SEQID 2
#define REQUESTSNIP_META 3

#define SNIP_STARTSNIP 15

#define SNIP_FRAMESNIP 16
#define FRAMESNIP_FRAME 1

#define SNIP_ACKSNIP 17

#define SNIP_NACKSNIP 18

#define SNIP_TIMEOUTSNIP 19

#define SNIP_ABORTSNIP 20

#define SNIP_EOSSNIP 21

#define HEADER_LENGTH 10

#define NETWORK_PORT 24567

void add_header(uint8_t* pInput, uint8_t* pOutput, int lengthInput);
int get_header(uint8_t *buffer, int offset, int *sniptyp, int *length);
int add_variant(uint8_t *buffer, int offset, int proto_id ,int value);
int add_lengthd(uint8_t *buffer, int offset, int proto_id ,uint8_t *data, long length);
int add_lengthd_empty(uint8_t *buffer, int offset, int proto_id);
int add_type(uint8_t *buffer, int offset, int typ);
int recv_ping(uint8_t *buffer, int offset, int *value);
int send_ping(uint8_t *buffer, int offset, int counter);
int recv_pong(uint8_t *buffer, int offset, int *value);
int send_pong(uint8_t *buffer, int offset, int counter);
int send_request(uint8_t *buffer, int offset, char *color, int seqId, uint8_t *meta, int offset_meta);
int create_metadata(uint8_t *buffer, int offset, uint32_t frames_per_second, uint32_t width, uint32_t heigtht, char *generator_name, char *generator_version);
int send_start(uint8_t *buffer, int offset);
int RGBValue(uint8_t *buffer, int offset, uint8_t red, uint8_t green, uint8_t blue, uint8_t x, uint8_t y);

#endif
