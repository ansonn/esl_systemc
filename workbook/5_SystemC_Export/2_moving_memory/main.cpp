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
    scx::scx_initialize("Dhrystone2");


    /*
     * Instantiate components
     */
    scx_evs_Dhrystone2 dhrystone2("Dhrystone2");

    //TODO: 5.2.2    instantiate  amba_pv::amba_pv_memory<BUSWIDTH> memory component


    /*
     * Parse arguments
     */
     scx::scx_parse_and_configure(argc, argv);
 

    /*
     * Parameters of the wrapped component
     */

    /* Semi-hosting configuration */
    scx::scx_set_parameter("*.Core.semihosting-enable", true);


    /*
     * Bindings
     */
     //TODO: 5.2.2 bind port ambapv_m of EVS to port amba_pv_s of the memory component
     //   <component.<master_port>(<component>.<slave_port>)


    /*
     * Start of simulation
     */
    sc_core::sc_start();


    return EXIT_SUCCESS;
}
