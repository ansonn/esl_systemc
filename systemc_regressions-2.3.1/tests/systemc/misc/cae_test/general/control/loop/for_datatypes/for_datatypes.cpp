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

  for_datatypes.cpp -- 

  Original Author: Rocco Jonack, Synopsys, Inc., 1999-07-29

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "for_datatypes.h"

#define max 10

void for_datatypes::entry()
{

  int  i;
  sc_signed   counter_signed(8);
  sc_unsigned counter_unsigned(8);

  // reset_loop
  if (reset.read()==true) {
    result.write(0);
    out_valid.write(false);
    wait();
  } else wait(); 

  //----------
  // main loop
  //----------
  while(1) {

    //read inputs
    while (in_valid.read()==false) wait();

    //execution of for loop with integer counter
    out_valid.write(true);
    wait();
    for (i=1; i<=max; i++) {
      result.write(in_value.read());
      wait();
    };
    out_valid.write(false);
    wait(4);

    //execution of for loop with signed counter
    out_valid.write(true);
    wait();
    for (counter_signed=1; counter_signed.to_int()<=max; counter_signed++) {
      result.write(in_value.read());
      wait();
    };
    out_valid.write(false);
    wait(4);

    //execution of for loop with unsinged counter
    out_valid.write(true);
    wait();
    for (counter_unsigned=1; counter_unsigned.to_uint()<=max; counter_unsigned++) {
      result.write(in_value.read());
      wait();
    };
    out_valid.write(false);
    wait();

  }
}

// EOF

