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
#include "data_ext.h"

int sc_main(int argc, char** argv) {
  scv_smart_ptr<data_t> data_p("data");

  // nbcode "seed" start
  //try to open the seed file to see if it exists
  FILE *seedfile = fopen("seedfile.txt", "r");

  if(seedfile) {
    //seedfile exists, use it
    scv_out << "Using seedfile.txt..." << endl;
    scv_random::seed_monitor_on(true, "seedfile.txt");
  } else {
    scv_out << "Saving seedfile.txt..." << endl;
    scv_random::seed_monitor_on(false, "seedfile.txt");
  }
  // nbcode "seed" end

  data_p->next();

  // nbcode "print" start
  scv_out << "Random value for data_p:" << endl;
  scv_out << data_p->field.get_name() << " = " << data_p->field << endl;
  scv_out << data_p->state.get_name() << " = " << data_p->state << endl;
  for(int i=0; i<5; ++i) {
    scv_out << data_p->payload[i].get_name() << " = "
            << data_p->payload[i] << endl;
  }
  // nbcode "print" end

  return 0;
}
