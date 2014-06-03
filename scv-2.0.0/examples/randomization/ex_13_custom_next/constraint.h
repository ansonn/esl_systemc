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
#include "scv.h"

// nbcode "decl" start
class my_constraint : public scv_constraint_base {
  protected:
    sc_signal<unsigned> *sig_;
    scv_smart_ptr<unsigned> val;

  public:
    scv_smart_ptr<unsigned> rval;
    // nbcode "decl" end

    // nbcode "ctor" start
    SCV_CONSTRAINT_CTOR(my_constraint) {
      val->disable_randomization();
      *val = 5;
      sig_ = NULL;
      SCV_CONSTRAINT( rval() < (val() + 5) );
      SCV_CONSTRAINT( rval() > (val() - 5) );
      SCV_CONSTRAINT( rval() != val() );
    }
    // nbcode "ctor" end

    // nbcode "methods" start
  public:
    void set_sig (sc_signal<unsigned>* sigp) { sig_ = sigp; }
    virtual void next() {
      if(sig_ != NULL) {
        *val = sig_->read();
      }
      scv_constraint_base::next();
    }
    // nbcode "methods" end
};
