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

  add_chain_main.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

// $Log: add_chain_main.cpp,v $
// Revision 1.2  2011/09/05 21:23:35  acg
//  Philipp A. Hartmann: eliminate compiler warnings.
//
// Revision 1.1.1.1  2006/12/15 20:26:13  acg
// systemc_tests-2.3
//
// Revision 1.4  2006/01/24 21:05:23  acg
//  Andy Goodrich: replacement of deprecated features with their non-deprecated
//  counterparts.
//
// Revision 1.3  2006/01/20 00:43:23  acg
// Andy Goodrich: Changed over to use putenv() instead of setenv() to accommodate old versions of Solaris.
//
// Revision 1.2  2006/01/19 00:47:31  acg
// Andy Goodrich: Changes for the fact signal write checking is enabled.
//

#include "common.h"
#include "add_chain.h"
#include "add_chain_tb.h" 	/** Definition of testbench Structure **/

int
sc_main(int ac, char *av[])
{
  sc_clock clk( "CLOCK", 20, SC_NS, 0.5, 10, SC_NS); // Clock function
  testbench tb1("TB1", clk );	// Testbench Instance
  sc_start();	 // Simulation runs forever 
					 // due to negative value
  return 0;
}
