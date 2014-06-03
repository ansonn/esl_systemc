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

  test.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "test.h"
 
void test::entry() 
{
  while (true) {
  o1 = 0;
  do { wait(); } while  (cont1 == 1);
  wait();
  wait ();
  if (i1 == 5) {
        if (i2 == 5) {
                do { wait(); } while  (cont2 == 1);
                wait ();
                o1 = 1;
        } else {
                wait();
                o1 = 2;
        }
  } else {
        wait();
        o1 = 3;
  }
  wait ();
  wait ();
  wait ();
  }
}

