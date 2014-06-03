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

  star103765.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include <systemc.h>
#include "test.h"

void test::reset_loop() {
  sc_uint<8> tmp;
  unsigned int i, j;
  
  wait();
  
  done = 0;
  dato = 0;
  tmp = 0;
  
  wait();
  operational_loop: while(1 != 0) {
    wait();
    
    block1 : for(i = 0; i < 1; i++) {
      tmp = tmp + 1;
      dato = tmp;
      wait();

      tmp = tmp + 1;
      if(tmp < 5) {
        break;
      } else {
        continue;
      }
      tmp = tmp + 1; // should never get here
    }
    wait();
    done_loop : while(1) {
      dato = tmp;
      done = 1;
      wait();
    }
  }
}

