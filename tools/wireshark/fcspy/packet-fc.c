/* @file packet-fc.c
 * @brief Wireshark disector implementation
 * @author Ollo
 *
 * @date 16.10.2013 – Logic to visualize Fullcircle packets
 * @defgroup Wireshark
 * 
 * Some usefull links:
 * - http://www.wireshark.org/docs/wsdg_html_chunked/ChDissectAdd.html
 *
 * Copyright 2013 C3MA. All rights reserved.
 */

/** @addtogroup Wireshark */
/*@{*/

#include "config.h"

#include <epan/packet.h>

#define FULLCIRCLE_PORT	24567

static int proto_fc = -1;

/** @fn static void dissect_fc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
 * @brief Generate a graphical representation of the transmit information
 * 
 * @param[in]	tvb		Databuffer representation
 * @param[in]	pinfo	Information about the actual disected packet
 * @param[in]	tree	Tree-node of wireshark, where the information can be added to
 * @return nonthing
 */
static void dissect_fc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree);

/** @fn void proto_register_fc(void)
 * @brief Initialize the library
 *
 * @return nothing
 */
void proto_register_fc(void)
{
	proto_fc = proto_register_protocol (
										 "Fullcircle Protocol", /* name       */
										 "FC",      /* short name */
										 "fc"       /* abbrev     */
										 );
}


void proto_reg_handoff_fc(void)
{
    static dissector_handle_t fc_handle;
	
    fc_handle = create_dissector_handle(dissect_fc, proto_fc);
    dissector_add_uint("tcp.port", FULLCIRCLE_PORT, fc_handle);
}

static void dissect_fc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "FC");
    /* Clear out stuff in the info column */
    col_clear(pinfo->cinfo,COL_INFO);
}

/*@}*/