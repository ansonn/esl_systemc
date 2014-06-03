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

  testbench.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

/* Filename testbench.cc */
/* This is the implementation file for synchronous process `testbench' */

#include <math.h>
#include "testbench.h"
#include "isaac.h"

QTIsaac<8> rng;

int add(int a, int b)
{
  return (a + b);
}

int adder_sub(int a, int b, int c, int *d)
{
  int sum;

  sum = add(a, b);
  sum = add(sum, c);
  *d = a - b;
  return (sum);
}

void testbench::entry()
{
  int a, b, c, d;
  int sum;
  int i;

  for (i=0; i < 10; i++) {
    a = rng.rand() & 0x0fffffff;
    b = rng.rand() & 0x0fffffff;
    c = rng.rand() & 0x0fffffff;

    sum = adder_sub(a, b, c, &d);
    // printf("A = %d, B = %d, C = %d, D = %d, SUM = %d\n", a, b, c, d, sum);
    char buf[BUFSIZ];
    sprintf(buf, "A = %d, B = %d, C = %d, D = %d, SUM = %d\n", a, b, c, d, sum);
    cout << buf;
  }
  sc_stop();
} // end of entry function

