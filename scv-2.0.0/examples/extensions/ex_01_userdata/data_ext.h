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
class scv_extensions<data_t> : public scv_extensions_base<data_t> {
public:
  scv_extensions<int         > field_1;
  scv_extensions<sc_uint<8>  > field_2;
  scv_extensions<unsigned[5] > payload;
  scv_extensions<std::string > str;

  SCV_EXTENSIONS_CTOR(data_t) {
    //must be in order
    SCV_FIELD(field_1);
    SCV_FIELD(field_2);
    SCV_FIELD(payload);
    SCV_FIELD(str);
  }
};
#endif //DATA_EXT_H
