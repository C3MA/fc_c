/* @file proto.h
 * @brief Another protobuf implementation
 * @author Pcopfer
 *
 * @date 26.06.2013 – Doxygen documentation added
 * @defgroup LowlevelProto
 *
 * Implementation of the necessary protobuf functionality.
 */

#ifndef fc_c_proto_h
#define fc_c_proto_h


/** @addtogroup LowlevelProto */
/*@{*/

#include <stdint.h>

#define PROTOTYPE_VARIANT   0
#define PROTOTYPE_LENGTHD   2


int parse(uint8_t *buffer, int offset, int* pId, int* pType);
int serialize(uint8_t *buffer, int offset, int id, int type);
int parse_number(uint8_t *buffer, int offset, int *value);
int serialize_number(uint8_t *buffer, int offset, int value);

/*@}*/

#endif
