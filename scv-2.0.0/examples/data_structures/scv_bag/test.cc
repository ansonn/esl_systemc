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
  //create a bag. Bags assist in the creation of different
  //random distributions
  scv_bag<packetT> bag("myBagOfPackets");
 // nbcode "decl" end

  // nbcode "init" start
  //throw some packets in the bag. We will create a squared
  //distribution of the elements.
  scv_smart_ptr<packetT> p; 
  p->length.keep_only (10, 1024); 
  for(unsigned int i=1; i<=11; ++i) {  //put something in for each src
    p->src=i-1;
    p->length.next(); p->dest.next();
    for(unsigned int j=0; j<p->length; ++j) {
      p->payload[j].next();
    }
    //our distribution is i**2, so the last elements added
    //have the highest probability of being fetched
    bag.add(*p, i*i);
  }

  //there should be 11 entries in the bag: 1st entry is 1
  //copy of the 1st element, 2nd entry is 4 copies of the
  //2nd element, 3rd entry is 9 copies of the 3rd element,
  //and so forth.  Total number of elements should be 506.
  scv_out << bag.get_name() << " has "  << bag.dSize() 
       << " entries and " << bag.size() << " elements" << endl;
  // nbcode "init" end

  // nbcode "entries" start
  //Let's peek into the bag. We will use the random peek,
  //and go until we have looked at everything
  scv_out << "Printing one element from each entry, "
          << "elements with the largest src have the\n"
          << "highest probability of printing first." << endl;
  while ( bag.unMarkedSize() ) {
    const packetT& bagElement = bag.peekRandom(); 
    bag.mark(true);         // Mark all elements in this entry
    scv_out << bagElement << endl;
  }
  // nbcode "entries" end
  // nbcode "elements" start
  //unmark all of the objects to use it again
  bag.unmarkAll(); 
  //This time let's peek into the bag 10 times without
  //marking.  Those elements with the largest src field
  //should appear most often.
  scv_out << "\nSelecting elements randomly, "
          << "elements with the largest src have the\n"
          << "highest probability of printing first." << endl;
  for (unsigned int i=0; i<10; ++i) {
    const packetT& bagElement = bag.peekRandom(); 
    scv_out << bagElement << endl;
  }
}
// nbcode "elements" end

int sc_main(int argc, char** argv) {
  sctop top("top");  //for osci
  sc_start();
  return 0;
}
