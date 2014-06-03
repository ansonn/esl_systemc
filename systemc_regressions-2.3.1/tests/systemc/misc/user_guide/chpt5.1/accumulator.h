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

  accumulator.h -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

/* Filename accumulator.h */
/* This is the interface file for synchronous process `accumulator' */

#include "systemc.h"

SC_MODULE( accumulator )
{
  SC_HAS_PROCESS( accumulator );

  sc_in_clk clk;

  const sc_signal<int>& number; //input
  sc_signal<int>& sum; //output
  sc_signal<int>& prod; //output

  int sum_acc; //internal variable
  int mult_acc; //internal variable

  //Constructor 
  accumulator(sc_module_name NAME,
	      sc_clock& CLK,
	      const sc_signal<int>& NUMBER,
	      sc_signal<int>& SUM,
	      sc_signal<int>& PROD)
    : number(NUMBER), sum(SUM), prod(PROD)
  {
    clk(CLK);
	SC_CTHREAD( entry, clk.pos() );
  }

  // Process functionality in member function below
  void entry();
};


