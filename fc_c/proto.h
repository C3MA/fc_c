//
//  proto.h
//  fc_c
//

#ifndef fc_c_proto_h
#define fc_c_proto_h

#include <stdint.h>

/** check if the offset correct, and the function above did not fail */
#define	CHECK_OFFSET_1( offset ) if ((offset) == -1) { return; }
#define	CHECK_OFFSET_2( offset, returnvalue ) if ((offset) == -1) { return (returnvalue); }
#define CHECK_OFFSET_X(x, A, B, FUNCT, ...) FUNCT
/* the value to return is optional, here the correct version from above is choosen */
#define CHECK_OFFSET(...) CHECK_OFFSET_X(__VA_ARGS__,\
											CHECK_OFFSET_2,\
											CHECK_OFFSET_1,\
										)
/* source: http://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros */

#define PROTOTYPE_VARIANT   0
#define PROTOTYPE_LENGTHD   2


int parse(uint8_t *buffer, int offset, int* pId, int* pType);
int serialize(uint8_t *buffer, int offset, int id, int type);
int parse_number(uint8_t *buffer, int offset, int *value);
int serialize_number(uint8_t *buffer, int offset, int value);

#endif
