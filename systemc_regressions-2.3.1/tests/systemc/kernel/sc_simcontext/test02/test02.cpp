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

  test02.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

// test of process initialization -- the dont_initialize() method

#include "systemc.h"

SC_MODULE( mod_a )
{
    sc_in_clk clk;

    void write( const char* msg )
    {
        cout << sc_time_stamp() << " " << msg << endl;
    }

    void method_a()
    {
        write( "method_a" );
    }

    void thread_a()
    {
        while( true ) {
            write( "thread_a" );
            wait();
        }
    }

    void cthread_a()
    {
        while( true ) {
            write( "cthread_a" );
            wait();
        }
    }

    SC_CTOR( mod_a )
    {
        SC_METHOD( method_a );
        sensitive << clk.neg();
        SC_THREAD( thread_a );
        sensitive << clk.neg();
        SC_CTHREAD( cthread_a, clk.neg() );
    }
};

SC_MODULE( mod_b )
{
    sc_in_clk clk;

    void write( const char* msg )
    {
        cout << sc_time_stamp() << " " << msg << endl;
    }

    void method_b()
    {
        write( "method_b" );
    }

    void thread_b()
    {
        while( true ) {
            write( "thread_b" );
            wait();
        }
    }

    void cthread_b()
    {
        while( true ) {
            write( "cthread_b" );
            wait();
        }
    }

    SC_CTOR( mod_b )
    {
        SC_METHOD( method_b );
        sensitive << clk.neg();
        dont_initialize();
        SC_THREAD( thread_b );
        sensitive << clk.neg();
        dont_initialize();
        SC_CTHREAD( cthread_b, clk.neg() );
        dont_initialize();
    }
};

int
sc_main( int, char*[] )
{
    sc_clock clk;

    mod_a a( "a" );
    mod_b b( "b" );

    a.clk( clk );
    b.clk( clk );

    sc_start( 3, SC_NS );

    return 0;
}
