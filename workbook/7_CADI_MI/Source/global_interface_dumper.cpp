/*
 * global_interface_dumper.h - dummy component dumping information provided by
 * the global interfaces
 *
 * Copyright 2011-2012 ARM Limited.
 * All rights reserved.
 */

/* Includes */
#include <eslapi/CADI.h>
#include <eslapi/CADIFactory.h>
#include <eslapi/CAInterfaceRegistryIF.h>
#include <MTI/ModelTraceInterface.h>
#include <sg/SGComponentRegistry.h>
#include <scx/scx.h> // for scx_get_global_interface()

#include "CADITest.h"

#include "global_interface_dumper.h"


/* Constructor */
global_interface_dumper::global_interface_dumper(sc_core::sc_module_name name):
sc_core::sc_module(name)
{        
}
    
/* Called just before the simulation starts */
void global_interface_dumper::start_of_simulation()
{
	cadi_start_thread(); 
	//sc_core::sc_stop(); 
}

