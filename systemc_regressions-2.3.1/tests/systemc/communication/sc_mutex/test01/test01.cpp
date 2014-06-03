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

  test01.cpp -- 

  Original Author: Martin Janssen, Synopsys, Inc., 2002-02-15

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

// test of the sc_mutex primitive channel

#include "systemc.h"

SC_MODULE( mod_a )
{
    sc_mutex mutex;

    void write( const char* msg )
    {
        cout << sc_time_stamp() << " " << msg << endl;
    }

    void proc_a()
    {
        while( true ) {
            wait( 1, SC_NS );
            write( "proc_a - lock requested" );
            mutex.lock();
            write( "proc_a - lock obtained" );
            wait( 2, SC_NS );
            if( mutex.unlock() == 0 ) {
                write( "proc_a - unlock successful" );
            } else {
                write( "proc_a - unlock failed" );
            }
            wait( 3, SC_NS );
            if( mutex.trylock() == 0 ) {
                write( "proc_a - trylock successful" );
            } else {
                write( "proc_a - trylock failed" );
            }
            if( mutex.unlock() == 0 ) {
                write( "proc_a - unlock successful" );
            } else {
                write( "proc_a - unlock failed" );
            }
        }
    }

    void proc_b()
    {
        while( true ) {
            wait( 2, SC_NS );
            write( "proc_b - lock requested" );
            mutex.lock();
            write( "proc_b - lock obtained" );
            wait( 4, SC_NS );
            if( mutex.unlock() == 0 ) {
                write( "proc_b - unlock successful" );
            } else {
                write( "proc_b - unlock failed" );
            }
            wait( 3, SC_NS );
            if( mutex.trylock() == 0 ) {
                write( "proc_b - trylock successful" );
            } else {
                write( "proc_b - trylock failed" );
            }
            if( mutex.unlock() == 0 ) {
                write( "proc_b - unlock successful" );
            } else {
                write( "proc_b - unlock failed" );
            }
        }
    }

    SC_CTOR( mod_a )
    {
        SC_THREAD( proc_a );
        SC_THREAD( proc_b );
    }
};

int
sc_main( int, char*[] )
{
    mod_a a( "a" );

    sc_start( 40, SC_NS );

    return 0;
}
