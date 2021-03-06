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
#ifndef PACKET_EXT_H
#define PACKET_EXT_H

#include "scv.h"
#include "packet.h"

template<>
class scv_extensions<packet_t> : public scv_extensions_base<packet_t> {
public:
  scv_extensions<sc_uint<32> > src_addr;
  scv_extensions<sc_uint<32> > dest_addr;
  scv_extensions<sc_uint<16> > msg_length;
  scv_extensions<char        [1024]> msg;

  SCV_EXTENSIONS_CTOR(packet_t) {
    //must be in order
    SCV_FIELD(src_addr);
    SCV_FIELD(dest_addr);
    SCV_FIELD(msg_length);
    SCV_FIELD(msg);
  }
};
#endif //PACKET_EXT_H
