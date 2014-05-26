/*
 * main.cpp - Dhrystone platform model wrapper with support for setting the
 *            quantum.
 *
 * Copyright 2012 ARM Limited.
 * All rights reserved.
 */

/* Includes */
#include <cstring>
#include <cstdlib>

#include <scx_evs_Dhrystone.h>

/* Globals */

static const char help_quantum[] =
    "-Q, --quantum N          "
    "number of ticks to simulate per quantum (default to 10000)\n";

/* Functions */

/*
 * Specific command-line options parsing
 */
double parse_quantum(int & argc , char * argv[]) {
    double q = 10000.0;
    int j = 1;

    for (int i = 1; (i < argc); i += 1) {
        if ((std::strcmp(argv[i], "-Q") == 0)
            || (std::strncmp(argv[i], "-Q=", 3) == 0)
            || (std::strcmp(argv[i], "--quantum") == 0)
            || (std::strncmp(argv[i], "--quantum=", 10) == 0)) {

            /* number of instructions to run per quantum */
            const char * p = strchr(argv[i], '=');

            if (p == NULL) {
                if ((i + 1) >= argc) {
                    std::cerr << argv[0] << ": option '" << argv[i]
                              << "' requires an argument\n";
                    std::exit(EXIT_FAILURE);
                }
                i += 1;
                q = std::atol(argv[i]);
            } else {
                q = std::atol(p + 1);
            }
            continue;
        }
        argv[j] = argv[i];
        j += 1;
    }
    argc = j;
    return (q);
}

/*
 * User's entry point.
 */
int sc_main(int argc , char * argv[]) {

    /*
     * Initialize simulation 
     */
    scx::scx_initialize("Dhrystone");

    /*
     * Components
     */
    amba_pv::amba_pv_memory<64> memory("Memory", 0x34000100);
    scx_evs_Dhrystone dhrystone("Dhrystone");

    /*
     * Number of instructions to run per quantum
     */
    double quantum = parse_quantum(argc, argv);

    /*
     * Simulation configuration
     */
   
    /* From command-line options */
    scx::scx_parse_and_configure(argc, argv, help_quantum);

    /* Semi-hosting configuration */
    bool v;

    if (scx::scx_get_parameter("Dhrystone.Core.cpu0.semihosting-enable", v)) {

        /* MP core */
        scx::scx_set_parameter("*.Core.cpu0.semihosting-enable", true);
        scx::scx_set_parameter("*.Core.cpu0.semihosting-ARM_SVC", 0x123456);
        scx::scx_set_parameter("*.Core.cpu0.semihosting-Thumb_SVC", 0xAB);
        scx::scx_set_parameter("*.Core.cpu0.semihosting-heap_base", 0x32000000);
        scx::scx_set_parameter("*.Core.cpu0.semihosting-heap_limit", 0x1000000);
        scx::scx_set_parameter("*.Core.cpu0.semihosting-stack_base", 0x33000000);
        scx::scx_set_parameter("*.Core.cpu0.semihosting-stack_limit", 0x1000000);
    } else {

        /* UP core */
        scx::scx_set_parameter("*.Core.semihosting-enable", true);
        scx::scx_set_parameter("*.Core.semihosting-ARM_SVC", 0x123456);
        scx::scx_set_parameter("*.Core.semihosting-Thumb_SVC", 0xAB);
        scx::scx_set_parameter("*.Core.semihosting-heap_base", 0x32000000);
        scx::scx_set_parameter("*.Core.semihosting-heap_limit", 0x1000000);
        scx::scx_set_parameter("*.Core.semihosting-stack_base", 0x33000000);
        scx::scx_set_parameter("*.Core.semihosting-stack_limit", 0x1000000);
    }

    /* Simulation quantum, i.e. seconds to run per quantum */
    tlm::tlm_global_quantum::instance().set(sc_core::sc_time(quantum
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
