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

  T_1_1_2_3.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "systemc.h"

SC_MODULE( foo )
{
    SC_HAS_PROCESS( foo );

    sc_in_clk clk;

    const sc_signal<bool> &input;
    sc_signal<bool> &output;

    int state;

    foo( sc_module_name NAME,
	 sc_clock& CLK,
	 const sc_signal<bool>& INPUT,
	 sc_signal<bool>& OUTPUT )
        : input(INPUT), output(OUTPUT)
    {
        clk(CLK);
		SC_CTHREAD( entry, clk.pos() );
    }
    void entry();
};

int sc_main(int argc, char* argv[] )
{
  return 0;
}
