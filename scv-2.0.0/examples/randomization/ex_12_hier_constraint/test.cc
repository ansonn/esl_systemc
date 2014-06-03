//  -*- C++ -*- <this line is for emacs to recognize it as C++ code>
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
#include "packet_constraint.h"

// nbcode "constraint" start
//create additional constraints
struct packet_hier_constraint : public packet_base_constraint {
  //add config variable
  scv_smart_ptr<sc_uint<32> > dest_min;
  scv_smart_ptr<sc_uint<32> > dest_max;
  
  SCV_CONSTRAINT_CTOR(packet_hier_constraint) {
    //use the base constraint
    SCV_BASE_CONSTRAINT(packet_base_constraint);

    //add extra constraints
    SCV_CONSTRAINT ((packet->dest_addr() > dest_min()) &&
                   (packet->dest_addr() < dest_max()) );
    SCV_CONSTRAINT (
      ((packet->src_addr() > (packet->dest_addr() + 0x100000) ) && 
       (packet->src_addr() < (packet->dest_addr() + 0x200000) ) ) || 
      ((packet->src_addr() < (packet->dest_addr() - 0x10000) ) &&
       (packet->src_addr() > (packet->dest_addr() - 0xfffff) )) );
  }
};
// nbcode "constraint" end

int sc_main(int argc, char** argv) {
  scv_random::set_global_seed(10);

  // nbcode "def" start
  //instatiate global constraints
  packet_hier_constraint pkt("Constrained Packet");

  pkt.dest_min->disable_randomization();
  pkt.dest_max->disable_randomization();
  *pkt.dest_min = 0x100000;
  *pkt.dest_max = 0x800000;
  // nbcode "def" end

  // nbcode "out" start
  for(int i=0; i<5; ++i) {
    pkt.next();
    pkt.packet->msg[(int)pkt.packet->msg_length] = '\0';
    scv_out << pkt.packet->get_name() << *(pkt.packet) << endl;
  }
  scv_out << endl;
  // nbcode "out" end

  return (0);
}


