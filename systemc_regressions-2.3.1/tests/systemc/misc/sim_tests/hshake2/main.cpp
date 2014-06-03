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

/* Main file for handshaking simulation */

#include "proc1.h"
#include "proc2.h"

int sc_main(int ac, char* av[])
{
  sc_signal<bool> data_ready("DataReady");
  sc_signal<bool> data_ack("DataAck");
  sc_signal<int> data("Data");

  sc_clock clock("CLOCK", 10, SC_NS, 0.5, 0.0, SC_NS);

  proc1 Master("MasterProcess", clock, data_ack, data, data_ready);
  proc2 Slave("SlaveProcess", clock, data_ready, data, data_ack);

  sc_start();
  cout << "SIMULATION COMPLETED AT TIME " << sc_time_stamp() << endl;
  return 0;
}
