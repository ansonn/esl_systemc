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

#include <iomanip>

void stateOut(scv_smart_ptr<data_t> data_p, int count[], onehot_t state) {
  scv_out << std::setw(0) << data_p->state.get_enum_string(state) << ": "
          << std::setw(4) << count[state] << endl;
}

// nbcode "decl" start
int sc_main(int argc, char** argv) {
  scv_smart_ptr<data_t> data_p("data");
  const int loopCount = 1000;
  int count[STATE_6+1];
  int total = 0;
  // nbcode "decl" end

  // nbcode "def" start
  //create a distribution for generating the state data
  //highest probabilty for states 1-4, zero chance for 3 and
  //lower probability for 0, 5, and 6.
  scv_bag<onehot_t> state_dist("state_dist");
  state_dist.add(STATE_0, 10);
  state_dist.add(STATE_1, 25);
  state_dist.add(STATE_2, 25);
  state_dist.add(STATE_4, 25);
  state_dist.add(STATE_5, 10);
  state_dist.add(STATE_6, 5);

  //set the distribution for the state variable in data_p
  data_p->state.set_mode(state_dist);
  // nbcode "def" end

  // nbcode "gen" start
  //initialize our counting array
  for (int i=0; i<STATE_6+1; i++) count[i] = 0;

  //randomize data loopCount times and track the results
  for(int i=0; i<loopCount; ++i) {
    data_p->next();
    count[data_p->state]++;
    total++;
  }
  // nbcode "gen" end

  // nbcode "out" start
  //print number of occurrences of each state
  stateOut(data_p, count, STATE_0);
  stateOut(data_p, count, STATE_1);
  stateOut(data_p, count, STATE_2);
  stateOut(data_p, count, STATE_3);
  stateOut(data_p, count, STATE_4);
  stateOut(data_p, count, STATE_5);
  stateOut(data_p, count, STATE_6);
  scv_out << "         ====" << endl;
  scv_out << "  Total: " << std::setw(4) << total << endl << endl;
  // nbcode "out" end

  return 0;
}
