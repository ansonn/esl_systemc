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

  stimulus.cpp -- 

  Original Author: Rocco Jonack, Synopsys, Inc., 1999-07-30

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "stimulus.h"

void stimulus::entry() {

    reset.write(true);
    wait();
    reset.write(false);

    sc_signed tmp1(8);
    sc_signed tmp2(8);
    long           tmp3;
    int            tmp4;
    short          tmp5;
    char           tmp6;
    char           tmp7;

    tmp1 = "0b01010101";
    tmp2 = "0b00000010";
    tmp3 = 12345678;
    tmp4 = -123456;
    tmp5 = 20000;
    tmp6 = '$'; 
    tmp7 = 'A';

    while(true){
       out_valid.write(true);
       out_value1.write(tmp1);
       out_value2.write(tmp2);
       out_value3.write(tmp3);
       out_value4.write(tmp4);
       out_value5.write(tmp5);
       out_value6.write(tmp6);
       out_value7.write(tmp7);
       cout << "Stimuli: " << tmp1 << " " << tmp2 << " " << tmp3 << " " << tmp4 << " "
	    << tmp5 << " " << tmp6 << " " << tmp7 << endl;
       tmp1 = tmp1 + 2;
       tmp2 = tmp2 + 1;
       tmp3 = tmp3 + 5;
       tmp4 = tmp4 + 3;
       tmp5 = tmp5 + 6;
       tmp7 = tmp7 + 1;
       do { wait(); } while (in_ack==false);
       out_valid.write(false);
       wait();
    }
}
// EOF
