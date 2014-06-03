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

  prime_numgen.h -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

                /***************************************/
                /* Interface Filename:	prime_numgen.h */
                /***************************************/

#include "common.h"

SC_MODULE( prime_numgen )
{
    SC_HAS_PROCESS( prime_numgen );

    sc_in_clk clk;

  // Inputs
        const sc_signal<bool>&	reset;
  // Outputs
        sc_signal<bool>&	prime_ready;
        signal_bool_vector&      prime;  

  // Constructor
  prime_numgen (sc_module_name		NAME,
	sc_clock&			TICK,
        const sc_signal<bool>&        	RESET,
        sc_signal<bool>&        	PRIME_READY,
        signal_bool_vector&      	PRIME )
	
      : 
	reset		(RESET),
	prime_ready	(PRIME_READY),
    	prime		(PRIME)

  	{
	    clk	(TICK); 
          SC_CTHREAD( entry, clk.pos() );
	  reset_signal_is(reset,false);
	}

  void entry();
};
