//  -*- C++ -*- <this line is for emacs to recognize it as C++ code>
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
#include "state_ext.h"

int sc_main(int argc, char** argv) {
  scv_smart_ptr<state_t> st_p("state_data");

  //set the values of the fields. Note: a scv_smart_ptr works like
  //a normal pointer object for accessing the object.
  st_p->state = state_t::STATE_3;
  st_p->delay = 2;
  st_p->data  = 0xaa;

  scv_out << "Printing data in object " << st_p->get_name() << endl;
  st_p->print(); //print the object

  return 0;
}

