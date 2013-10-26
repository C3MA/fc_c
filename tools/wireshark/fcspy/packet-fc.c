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
#include <epan/column-utils.h>
#include <epan/packet.h>

#define FULLCIRCLE_PORT	24567

static int proto_fc = -1;

/* all fields to display needs an handle */
static int hf_dynfc_length = -1;

/* Initialize the subtree pointers */
static gint ett_dynfc = -1;


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
	/* Setup list of header fields  See Section 1.6.1 for details*/
    static hf_register_info hf[] =
    {
	// All the general fields for the header
        { &hf_dynfc_length,                 { "Length",                       "dynfc.length", FT_STRING, BASE_NONE, NULL, 0x0,     "", HFILL } },

    };
    /* Setup protocol subtree array */
    static gint *ett[] = {
        &ett_dynfc,
    };
	proto_fc = proto_register_protocol (
										 "Dynamic Fullcircle Protocol", /* name       */
										 "DynFC",      /* short name */
										 "dynfc"       /* abbrev     */
										 );
	register_dissector("DynFC", dissect_fc, proto_fc); 	

	/* Required function calls to register the header fields and subtrees used */
	proto_register_field_array(proto_fc, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

}


void proto_reg_handoff_fc(void)
{
    static dissector_handle_t fc_handle;
	
    fc_handle = create_dissector_handle(dissect_fc, proto_fc);
    dissector_add_uint("tcp.port", FULLCIRCLE_PORT, fc_handle);
}

static void dissect_fc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    proto_tree* fc_tree = NULL;
    proto_item *ti;

    /* Set the detected protocol */
    if (check_col(pinfo->cinfo, COL_PROTOCOL))
    {
        col_set_str(pinfo->cinfo, COL_PROTOCOL, "DynFC");
    }

    /* Clear out stuff in the info column */
   if (check_col(pinfo->cinfo, COL_INFO))
   {
        col_clear(pinfo->cinfo, COL_INFO);
   }
  
  ti = proto_tree_add_item(tree, proto_fc, tvb, 0, -1, FALSE);
  fc_tree = proto_item_add_subtree(ti, ett_dynfc);

  ti = proto_tree_add_item (fc_tree, hf_dynfc_length, tvb, 0, 10, FALSE); 
}

/*@}*/
