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

  stepwise_simulation.cpp -- Test step-wise simulation

  Original Author: Philipp A. Hartmann, OFFIS, 2011-01-21

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#include <systemc.h>

const int num_events = 2;
const sc_time delay( 10, SC_NS );

//#define EXPLICIT_DELTA // should not modify the trace
#define DELAYED_EVENTS

SC_MODULE(echo)
{
  sc_event ev[num_events];
  SC_CTOR(echo)
    : self_trigger(3)
  {
    SC_METHOD(do_print);
#   ifdef DELAYED_EVENTS
      dont_initialize();
#   endif
      for( int i=0; i<num_events; ++i )
        sensitive << ev[i];
  }

  void do_print()
  {
    std::cout << "`" << sc_get_current_process_handle().name()
              << "' triggered at " << sc_time_stamp()
              << " - delta: " << sc_delta_count()
              << std::endl;

    if( --self_trigger > 1 )
      next_trigger( delay/2 );
    else if( self_trigger == 1 )
      next_trigger( SC_ZERO_TIME );
    else
      self_trigger = 3;

  }
  int self_trigger;
};

void do_step( sc_time const & step )
{
    sc_dt::uint64 delta       = sc_delta_count();
    static bool   start_delta = ( delta == 0 ) && ( step == SC_ZERO_TIME );
    std::cout
       << " ----- running for "
       << ( ( step == SC_ZERO_TIME ) ? "delta" : step.to_string() )
       << std::endl;

    sc_time start = sc_time_stamp();
    if( step > SC_ZERO_TIME ) 
    {
      std::cout << "   --- No-op start (warning) - ";
      sc_start( step / 2, SC_EXIT_ON_STARVATION );
      sc_assert( start == sc_time_stamp() );

      sc_start( step / 2, SC_RUN_TO_TIME );
      sc_assert( start + step / 2 == sc_time_stamp() );

      sc_start( step / 2 ); // complete step
    }
    else
    {
      sc_start( SC_ZERO_TIME );
    }
    sc_assert( start + step == sc_time_stamp() );

    std::cout
       << "    -- stopped at - "
       << sc_time_stamp() << " - delta: " << sc_delta_count()
       << std::endl;

    // delta has only increased, when a delta has been run
    sc_assert( sc_delta_count()
                 == delta + ( !start_delta && step == SC_ZERO_TIME ) );
    start_delta = false;
}

int sc_main(int, char*[])
{
  sc_assert( !sc_pending_activity() );
  sc_assert( sc_time_to_pending_activity()
             == sc_max_time() - sc_time_stamp() );

  echo dut("echo");

  // notify future events
  for( int i=0; i<num_events; ++i )
    dut.ev[i].notify( (i+1) * delay );

  sc_assert( sc_pending_activity_at_future_time() );
  sc_assert( sc_time_to_pending_activity() == delay );

  do_step( SC_ZERO_TIME ); // elaborate

  while( sc_pending_activity() )
  {
    sc_assert( sc_pending_activity_at_current_time()
               || sc_time_to_pending_activity() > SC_ZERO_TIME );

    sc_assert( sc_pending_activity_at_future_time()
               || sc_time_to_pending_activity() == SC_ZERO_TIME );

    // run single (time) step
    do_step( sc_time_to_pending_activity() );

#   ifdef EXPLICIT_DELTA
      // run remaining current deltas (optional)
      while( sc_pending_activity_at_current_time() ) {
        sc_assert( sc_time_to_pending_activity() == SC_ZERO_TIME );
        do_step( SC_ZERO_TIME );
      }
#   endif // EXPLICIT_DELTA
  }

  sc_assert( !sc_pending_activity() );
  sc_assert( sc_time_to_pending_activity()
             == sc_max_time() - sc_time_stamp() );

  std::cout << "Success" << std::endl;
  return 0;
}
