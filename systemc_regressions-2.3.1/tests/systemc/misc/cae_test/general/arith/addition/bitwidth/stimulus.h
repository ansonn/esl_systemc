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

  stimulus.h -- 

  Original Author: Rocco Jonack, Synopsys, Inc., 1999-08-02

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include "common.h"

SC_MODULE( stimulus )
{
  SC_HAS_PROCESS( stimulus );

  sc_in_clk clk;

  sc_signal<bool>&  reset;
  sc_signal_bool_vector4&      out_stimulus1;
  sc_signal_bool_vector4&      out_stimulus2;
  sc_signal_bool_vector6&      out_stimulus3;
  sc_signal_bool_vector6&      out_stimulus4;
  sc_signal_bool_vector8&      out_stimulus5;
  sc_signal_bool_vector8&      out_stimulus6;
  sc_signal<bool>&  out_valid;

  stimulus(sc_module_name           NAME,
	   sc_clock&                CLK,
	   sc_signal<bool>&         RESET,
	   sc_signal_bool_vector4&  OUT_STIMULUS1,
	   sc_signal_bool_vector4&  OUT_STIMULUS2,    
	   sc_signal_bool_vector6&  OUT_STIMULUS3,    
	   sc_signal_bool_vector6&  OUT_STIMULUS4,      
	   sc_signal_bool_vector8&  OUT_STIMULUS5,     
	   sc_signal_bool_vector8&  OUT_STIMULUS6,     
	   sc_signal<bool>&         OUT_VALID
          )
          : 
            reset(RESET),
            out_stimulus1(OUT_STIMULUS1),
            out_stimulus2(OUT_STIMULUS2),
            out_stimulus3(OUT_STIMULUS3),
            out_stimulus4(OUT_STIMULUS4),
            out_stimulus5(OUT_STIMULUS5),
            out_stimulus6(OUT_STIMULUS6),
            out_valid(OUT_VALID)
  {
    clk (CLK);
	SC_CTHREAD( entry, clk.pos() );
  }
  
  void entry();
};

// EOF
