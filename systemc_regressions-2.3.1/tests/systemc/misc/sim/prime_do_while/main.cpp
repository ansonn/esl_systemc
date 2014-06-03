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

  main.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

                /***************************************/
                /* Main Filename: 	main.cc        */
                /***************************************/

#include "reset.h"
#include "display.h"
#include "prime_numgen.h"

int
sc_main(int ac, char *av[])
{

// Signal Instantiation
  sc_signal<bool>         reset	("reset");
  signal_bool_vector      prime	("prime");

// Clock Instantiation
  sc_clock 	clk ("CLK", 6, SC_NS, 0.5, 10, SC_NS, false);	// 167 Mhz

// Process Instantiation
  prime_numgen	D1 ("D1", clk, reset, prime);

  resetp	T1 ("T1", clk, reset);
 
  displayp	T2 ("T2", clk, prime);
 
// Simulation Run Control
  sc_start();
 return 0;
}
