/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2006 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.4 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  fifo.h - This is the header file for the SystemC structure "fifo".

  Original Author: Rashmi Goswami, Synopsys, Inc.

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#include "systemc.h"
#include "pkt.h"

struct fifo {

   pkt regs[4];
   bool full;
   bool empty;
   sc_uint<3> pntr;
  
   // constructor

   fifo()
    {
      full = false;
      empty = true;
      pntr = 0;
    }

  // methods
   
   void pkt_in(const pkt& data_pkt);
   
   pkt pkt_out();
};
#endif // FIFO_H_INCLUDED
