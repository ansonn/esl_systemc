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

  Original Author: Bishnupriya Bhattacharya, Cadence Design Systems, 
                   September 5, 2003

 *****************************************************************************/

// test error message for wait() on dynamic method process handle's event.

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Andy Goodrich, Forte Design Systems, 26 Jul 2005
  Description of Modification: Changed waits to use the new terminated_event
                               support.

 *****************************************************************************/
// $Log: test03.cpp,v $
// Revision 1.2  2011/02/01 17:17:40  acg
//  Andy Goodrich: update of copyright notice, added visible CVS logging.
//

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc.h>

int function_method(double d)
{
  cout << endl << sc_time_stamp() << ", " 
       << sc_get_current_process_handle().name() 
       << ": function_method sees " << d << endl;
  return int(d);
}

class module1 : public sc_module
{
private:
  sc_event& ev;
public:

  SC_HAS_PROCESS(module1);

  module1(sc_module_name name, sc_event& event) : sc_module(name), 
    ev(event)
  {
    SC_METHOD(static_method);
  }

  void static_method() {
    int r;
    cout << endl << sc_time_stamp() << ": static_method, Before spawning function_method " << endl;
    sc_spawn_options o1;
    o1.spawn_method();
    o1.dont_initialize();
    o1.set_sensitivity(&ev);
    sc_process_handle h4 = sc_spawn(&r, sc_bind(&function_method, 1.2345), "event_sensitive_method", &o1);
    wait(h4.terminated_event());
  }
};

int sc_main (int argc , char *argv[]) 
{
  sc_event event1;
  event1.notify(55, SC_NS);

  module1 mod1("mod1", event1);
  sc_start(100, SC_NS);
  return 0;
}
