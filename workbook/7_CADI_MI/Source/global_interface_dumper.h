/*
 * global_interface_dumper.h - dummy component dumping information provided by
 * the global interfaces
 *
 * Copyright 2011-2012 ARM Limited.
 * All rights reserved.
 */

#ifndef GLOBAL_INTERFACE_DUMPER_H_
#define GLOBAL_INTERFACE_DUMPER_H_

/* Includes */
#include <systemc>

class global_interface_dumper: public sc_core::sc_module
{
public:
    explicit global_interface_dumper(sc_core::sc_module_name name);
    virtual void start_of_simulation();
};

#endif 

