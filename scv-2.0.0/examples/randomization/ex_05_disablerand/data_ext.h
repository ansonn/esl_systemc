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
#ifndef DATA_EXT_H
#define DATA_EXT_H

#include "scv.h"
#include "data.h"


//declarations below this point are due to the --EXTEND_ALL option

template<>
class scv_extensions<onehot_t> : public scv_enum_base<onehot_t> {
public:

  SCV_ENUM_CTOR(onehot_t) {
    SCV_ENUM(STATE_0);
    SCV_ENUM(STATE_1);
    SCV_ENUM(STATE_2);
    SCV_ENUM(STATE_3);
    SCV_ENUM(STATE_4);
    SCV_ENUM(STATE_5);
    SCV_ENUM(STATE_6);
  }
};

template<>
class scv_extensions<data_t> : public scv_extensions_base<data_t> {
public:
  scv_extensions<sc_uint<8> > field;
  scv_extensions<unsigned   [5]> payload;
  scv_extensions<onehot_t   > state;

  SCV_EXTENSIONS_CTOR(data_t) {
    //must be in order
    SCV_FIELD(field);
    SCV_FIELD(payload);
    SCV_FIELD(state);
  }
};
#endif //DATA_EXT_H
