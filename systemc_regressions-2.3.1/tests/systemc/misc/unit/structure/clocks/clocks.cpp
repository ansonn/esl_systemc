/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2014 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.accellera.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  clocks.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "systemc.h"
#include "clocks.h"
#include "tb.h" 	/** Definition of testbench Structure **/

int
sc_main( int, char*[] )
{
    sc_clock clk( "CLK", 20, SC_NS, 0.5, 0, SC_NS);
    sc_clock clk2( "_____________CLK2", 20, SC_NS, 0.2, 5, SC_NS);

    testbench tb1("TB1", clk, clk2);	

    sc_start( 80, SC_NS );          // Simulation control
    return 0;
}
