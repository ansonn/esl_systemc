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

  //set the global seed to that results will be consistent
  scv_random::set_global_seed(100);

        // nbcode "disable" start
  //set the values of the array element
  for(unsigned i=0; i<5; ++i) data_p->payload[i] = i*i;

  //turn off randomization for the even array elements
  for(unsigned i=0; i<5; i+=2) 
    data_p->payload[i].disable_randomization();
  // nbcode "disable" end

  //randomize composit
  data_p->next();

  scv_out << "Random value for " << data_p->get_name() << ":" << endl;
  data_p->print(scv_out);

  return 0;
}


