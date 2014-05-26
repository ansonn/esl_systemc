/*
 * Dhrystone2: main.cpp - Dhrystone2 platform model wrapper.
 *
 * The confidential and proprietary information contained in this file may
 * only be used by a person authorised under and to the extent permitted
 * by a subsisting licensing agreement from ARM Limited.
 *
 * Copyright 2007-2009 ARM Limited.
 * All rights reserved.
 *
 * This entire notice must be reproduced on all copies of this file
 * and copies of this file may only be made by a person if such person is
 * permitted to do so under the terms of a subsisting license agreement
 * from ARM Limited.
 */

/*
 * Revision $Revision: 37318 $
 * Checkin $Date: 2010-04-23 18:51:03 +0100 (Fri, 23 Apr 2010) $
 * Revising $Author: anukha01 $
 */

/* Includes */
#include <sys/stat.h>

// TODO: 5.1.2  Add header file for the EVS
#include "scx_evs_Dhrystone2.h"

#include "tlm.h"
#include "types.h"


/*
 * User's entry point.
 */
int sc_main (int argc , char *argv[]) {

    /*
     * Initialize simulation 
     */
     //TODO: 5.1.2   initialize the simulation using scx_initialize()
    scx::scx_initialize("Dhrystone2");


    /*
     * Instantiate components
     */
     //TODO: 5.1.2   instantiate sc_sg_wrapper_Dhrystone2 object
    scx_evs_Dhrystone2 dhrystone2("Dhrystone2");


    /*
     * Parse arguments
     */
     //TODO: 5.1.2   Add the scx_parse_and_configure() function
     scx::scx_parse_and_configure(argc, argv);
 

    /*
     * Parameters of the wrapped component
     */

    /* Semi-hosting configuration */
    //TODO: 5.1.2
    // Fast Models user guide '5.6.7 Parameter access functions
    scx::scx_set_parameter("*.Core.semihosting-enable", true);


    /*
     * Start of simulation
     */
    sc_core::sc_start();


    return EXIT_SUCCESS;
}
