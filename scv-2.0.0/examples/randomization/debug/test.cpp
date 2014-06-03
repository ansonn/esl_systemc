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
// DEBUG EXAMPLE

#include "scv.h"

/////////////////////////////////////////////////
//
// Define a constraint class with an unsatisfiable constraint
//    a() > 0 && a() < 0
//
// User can use the constraint_object.print() command to print
// the constraint expressions (both soft and hard) and also
// values of all the variables in the constraint class.
//
// Note the names of the scv_smart_ptr objects should be set
// in the constructor for the expression to print proper names.
// "default" is used in the expression string for any scv_smart_ptr
// member whose name is not set by the user.
////////////////////////////////////////////////
class constraint : public scv_constraint_base {
public:
  scv_smart_ptr<int> a;
public:
  SCV_CONSTRAINT_CTOR(constraint) {
    a->set_name("a");
    SCV_CONSTRAINT( a() > 0 && a() < 0 );
  }
};

///////////////////////////////////////////////////////
//
// Define a constraint class constraint_runtime
//
// Contains 2 hard constraints and 1 soft constraint
// Note that in the test after the initial 5 iterations for generating
// values for object c1, value of member b is set to 4 which  invalidates
// the soft constraint.
//
// Users can use print on constraint objects to print original constraint
// expressions and also the current values of the member variables, both 
// before and after every call to generating new random value.
///////////////////////////////////////////////////////

class constraint_runtime : public scv_constraint_base {
public:
  scv_smart_ptr<int> a;
  scv_smart_ptr<int> b;
public:
  SCV_CONSTRAINT_CTOR(constraint_runtime) {
    a->set_name("a");
    b->set_name("b");
    SCV_CONSTRAINT( a() > 5 && a() < 10 );
    SCV_CONSTRAINT( b() > 0 && b() < 10 );
    SCV_SOFT_CONSTRAINT( a() == b() );
  }
};

int sc_main(int argc, char ** argv)
{
  constraint c("c");

  // Creating the constraint object c will result in overconstraint error
  // message.  Can use print statement to print the constraint expressions 
  // and values of the member variables.

  c.print();

  constraint_runtime c1("c1");
  // Generate values for constraint object c1
  for (int i=0; i < 5; i++) {
    c1.next();
    cout << "c1.a: " << *c1.a << " c1.b" << *c1.b << endl;
  }

  // explicitly assign value 4 c1.b
  *c1.b = 4;

  // If you generate new value for c1.a it will invalidate the soft
  // constraint.
  c1.print();

  // Will result in ignoring soft constraint
  c1.a->next();

  return 0;
}

