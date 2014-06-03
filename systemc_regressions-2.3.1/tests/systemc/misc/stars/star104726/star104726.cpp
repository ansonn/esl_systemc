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

  star104726.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "quant.h"

	
void quant::do_quant()
{
   Coeff8x8 c;
   Coeff8x8 fuv;
   COEFF    ff;
   int      u,v;
  
  while(true) {

    ready.write(true);
    data_out_ready.write(false);
    do { wait(); } while (start==false);
    ready.write(false);

    c = data_in.read();

     // quantization
      for( v=0; v<8; v++ ) {
        for( u=0; u<8; u++ ) {
          ff = (c.get(v,u)<<1) / (COEFF)(coeff_quant[v][u]);
          fuv.put(v,u,(ff<0 ? ff : ff+1) >> 1);
        }
      }
  
     data_out.write(fuv);

    data_out_ready.write(true);
    do { wait(); } while (data_ok==false);

  }
}


