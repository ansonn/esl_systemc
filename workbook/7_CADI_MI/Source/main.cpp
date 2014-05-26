/*
 * main.cpp - Dhrystone platform model wrapper demonstrating access to the
 *            global interfaces
 *
 * Copyright 2011-2012 ARM Limited.
 * All rights reserved.
 */

/* Includes */
#include <scx_evs_Dhrystone.h>
#include "global_interface_dumper.h"

/* Functions */

/*
 * User's entry point.
 */
int sc_main(int argc , char * argv[]) {

    /*
     * Initialize simulation 
     */
    scx::scx_initialize("GlobalInterface");

    /*
     * Components
     */
    amba_pv::amba_pv_memory<64> memory("Memory", 0x34000100);
    scx_evs_Dhrystone dhrystone("Dhrystone");
    global_interface_dumper dumper("dumper");

    /*
     * Simulation configuration
     */
   
    /* From command-line options */
    scx::scx_parse_and_configure(argc, argv);

    /* Semi-hosting configuration */
    scx::scx_set_parameter("*.Core.cpu0.semihosting-enable", true);
    scx::scx_set_parameter("*.Core.cpu0.semihosting-ARM_SVC", 0x123456);
    scx::scx_set_parameter("*.Core.cpu0.semihosting-Thumb_SVC", 0xAB);
    scx::scx_set_parameter("*.Core.cpu0.semihosting-heap_base", 0x32000000);
    scx::scx_set_parameter("*.Core.cpu0.semihosting-heap_limit", 0x1000000);
    scx::scx_set_parameter("*.Core.cpu0.semihosting-stack_base", 0x33000000);
    scx::scx_set_parameter("*.Core.cpu0.semihosting-stack_limit", 0x1000000);

    /* Simulation quantum, i.e. seconds to run per quantum */
    tlm::tlm_global_quantum::instance().set(sc_core::sc_time(10000.0
                                                             / 100000000,
                                                             sc_core::SC_SEC));

    /*
     * Bindings
     */
    dhrystone.amba_pv_m(memory.amba_pv_s);

    /*
     * Start of simulation
     */
    sc_core::sc_start();
    return EXIT_SUCCESS;
}

