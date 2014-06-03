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
#include "scv.h"
#include "packet.h"

SC_MODULE(sctop) {
  SC_CTOR(sctop);
};

// nbcode "decl" start
sctop::sctop(sc_module_name name) : sc_module(name)
{
  // Create a smart pointer for each packet type
  scv_smart_ptr<packetT1> p1;
  scv_smart_ptr<packetT2> p2;
  // nbcode "decl" end

  // nbcode "first" start
  p1->next();
  p2->next();

  scv_out << "\nPacket values after randomization:" << endl;
  scv_out << "p1: " << *p1 << endl;
  scv_out << "p2: " << *p2 << endl;
  // nbcode "first" end

  // nbcode "second" start
  p1->next();
  p2->next();
  scv_smart_ptr< sc_uint<16> > rand;
  rand->next(); (p2->get_instance())->set_src (rand->read());
  rand->next(); (p2->get_instance())->set_dest(rand->read());

  scv_out << "\nPacket values after randomization using rand:" << endl;
  scv_out << "p1: " << *p1 << endl;
  scv_out << "p2: " << *p2 << endl;
}
// nbcode "second" end

int sc_main(int argc, char** argv) {
  sctop top("top");  //for osci
  sc_start();
  return 0;
}
