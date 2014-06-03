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

  bitwidth.h -- 

  Original Author: Rocco Jonack, Synopsys, Inc., 1999-07-14

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/


#include "common.h"

SC_MODULE( bitwidth )
{
    SC_HAS_PROCESS( bitwidth );

    sc_in_clk clk;

    //====================================================================
    // [C] Always Needed Member Function
    //        --  constructor
    //        --  entry
    //====================================================================

    const sc_signal<bool>&             reset ;
    const sc_signal_bool_vector2&      in_value1;     // Input  port
    const sc_signal_bool_vector4&      in_value2;     // Input  port
    const sc_signal_bool_vector6&      in_value3;     // Input  port
    const sc_signal_bool_vector8&      in_value4;     // Input  port
    const sc_signal<bool>&             in_valid;      // Input  port
    sc_signal_bool_vector2&            out_value1;    // Output port
    sc_signal_bool_vector4&            out_value2;    // Output port
    sc_signal_bool_vector6&            out_value3;    // Output port
    sc_signal_bool_vector8&            out_value4;    // Output port
    sc_signal<bool>&                   out_ack;       // Output port
    sc_signal<bool>&                   out_valid;     // Output port


    //
    // Constructor
    //

    bitwidth (
               sc_module_name   NAME,          // referense name
               sc_clock&        CLK,           // clock
        const  sc_signal<bool>& RESET,
        const  sc_signal_bool_vector2&            IN_VALUE1,
        const  sc_signal_bool_vector4&            IN_VALUE2,
        const  sc_signal_bool_vector6&            IN_VALUE3,
        const  sc_signal_bool_vector8&            IN_VALUE4,
        const  sc_signal<bool>&                   IN_VALID,     // Input  port
               sc_signal_bool_vector2&            OUT_VALUE1,
               sc_signal_bool_vector4&            OUT_VALUE2,
               sc_signal_bool_vector6&            OUT_VALUE3,
               sc_signal_bool_vector8&            OUT_VALUE4,
               sc_signal<bool>&                   OUT_ACK,
               sc_signal<bool>&                   OUT_VALID    // Output port
        )
        : 
          reset        (RESET),
          in_value1    (IN_VALUE1),
          in_value2    (IN_VALUE2),
          in_value3    (IN_VALUE3),
          in_value4    (IN_VALUE4),
          in_valid     (IN_VALID),
          out_value1   (OUT_VALUE1),
          out_value2   (OUT_VALUE2),
          out_value3   (OUT_VALUE3),
          out_value4   (OUT_VALUE4),
          out_ack      (OUT_ACK),
          out_valid    (OUT_VALID)

    {
      clk          (CLK);
	  SC_CTHREAD( entry, clk.pos() );
      reset_signal_is(reset,true);
    };

    //

void entry ();

};

// EOF


