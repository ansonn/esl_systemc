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
#include "constraint.h"

#include <iomanip>

// nbcode "head" start
SC_MODULE(test) {
  sc_signal<unsigned> addr;
  my_constraint cnstr;
  // nbcode "head" end

  // nbcode "ctor" start
  SC_CTOR(test) : cnstr("cnstr") {
    cnstr.set_sig(&addr);
    SC_THREAD(run);
  }
  // nbcode "ctor" end

  // nbcode "method" start
  void run() {
    unsigned i;
    for(i=10; i<100; i+=20) {
      addr=i;
      scv_out << "addr = " << std::setw(2) << addr << ", Setting to "
              << std::setw(2) << i << endl;
      wait(0, SC_NS);
      cnstr.next();
      scv_out << "addr = " << std::setw(2) << addr << ", rval = "
              << std::setw(2) << *cnstr.rval << "\n" << endl;
      wait(10, SC_NS);
    }
    sc_stop();
  }
  // nbcode "method" end
};
