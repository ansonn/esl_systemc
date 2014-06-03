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

/* Main file for memory simulation */

#include "accessor.h"
#include "ram.h"

int global_mem[4000];

int sc_main(int ac, char *av[])
{
  sc_signal<bool> cs("CS");
  sc_signal<bool> we("WE");
  signal_bool_vector10 addr("Address");
  signal_bool_vector32 data1("Data1");
  signal_bool_vector32 data2("Data2");

  sc_clock clk("Clock", 20, SC_NS, 0.5, 0.0, SC_NS);

  accessor A("Accessor", clk, data1, cs, we, addr, data2);
  ram R("Ram", data2, cs, we, addr, data1);

  sc_start(560, SC_NS);
  return 0;
}
