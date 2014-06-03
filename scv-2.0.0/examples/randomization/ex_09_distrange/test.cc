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

// nbcode "decl" start
int sc_main(int argc, char** argv) {
  scv_smart_ptr<data_t> data_p("data");
  scv_random::set_global_seed(666);
  // nbcode "decl" end

  // nbcode "track_decl" start
  #define FIELD_PAIR std::pair<sc_uint<8>, sc_uint<8> >
  class rangeDef {
  public:
    FIELD_PAIR range;
    int percentage, count;
    bool InRange(sc_uint<8> val)
    {
      return (val >= range.first && val <= range.second);
    }
    rangeDef() {count = 0;}
  };
  const int numRanges = 4;
  rangeDef ranges[numRanges];
  int total = 0;
  const int loopCount = 1000;
  // nbcode "track_decl" end

  // nbcode "def" start
  //create a non-linear approximation for field using ranges,
  //initialize tracking count
  ranges[0].range = FIELD_PAIR(0x00, 0x02);  // 0-2
  ranges[1].range = FIELD_PAIR(0x03, 0x06);  // 3-6
  ranges[2].range = FIELD_PAIR(0x07, 0x0f);  // 7-15
  ranges[3].range = FIELD_PAIR(0x10, 0xff);  // 16-255

  ranges[0].percentage = 60;
  ranges[1].percentage = 20;
  ranges[2].percentage = 10;
  ranges[3].percentage = 10;

  //declare and fill our bag with the distribution information
  scv_bag< FIELD_PAIR > field_dist;
  for (int i=0; i<numRanges; i++) {
    field_dist.add(ranges[i].range, ranges[i].percentage);
  }

  //set the distribution for the field variable in data_p
  data_p->field.set_mode(field_dist);
  // nbcode "def" end

  // nbcode "gen" start
  //randomize data loopcount times and print the results
  for(int i=0; i<loopCount; ++i) {
    data_p->next();
    for (int j=0; j<numRanges; j++) {
      if (ranges[j].InRange(data_p->field.read())) {
        ranges[j].count++;
        total++;
        break;
      }
    }
  }
  // nbcode "gen" end

  // nbcode "print" start
  //print distribution
  for (int i=0; i<numRanges; i++) {
    scv_out << "Range " << std::setw(2) << i << ": "
            << std::setw(3) << ranges[i].range.first << "-"
            << std::setw(3) << ranges[i].range.second << "  "
            << std::setw(4) << ranges[i].count << endl;
  }
  scv_out << "                   ====" << endl;
  scv_out << "            Total: " << std::setw(4) << total << endl << endl;
  // nbcode "print" end

  return 0;
}
