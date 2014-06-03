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
/*****************************************************************************
 constraints.h -- 
  
 Define a constraint class to illustrate notion of dependent variables
 and explain use_constraint() and next() on dependent object semantics.
  
 *****************************************************************************/

#ifndef _CONSTRAINT_H
#define _CONSTRAINT_H

class dependent_integer_constraints : public scv_constraint_base {
public:
  scv_smart_ptr<int> ivar;
  scv_smart_ptr<int> jvar;
public:
  SCV_CONSTRAINT_CTOR(dependent_integer_constraints) {
    SCV_CONSTRAINT(ivar() >= 3 && ivar() <= 12);
    SCV_CONSTRAINT(jvar() >= 3 && jvar() <= 12);
    SCV_SOFT_CONSTRAINT(ivar() > jvar() );
  }
};

inline void print_constraint(dependent_integer_constraints& c)
{
  cout << "  ivar: " << *c.ivar << " jvar: " << *c.jvar << endl;
}

#endif // _CONSTRAINT_H
