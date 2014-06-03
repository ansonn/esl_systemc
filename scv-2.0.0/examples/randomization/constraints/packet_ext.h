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
 packet_ext.h --
  
 File created using tb_wizard from the "packet" data type defined in packet.h 

 *****************************************************************************/

#ifndef PKT_EXT_H
#define PKT_EXT_H

#include "packet.h"

SCV_ENUM_EXTENSIONS(packet_types_t) {
 public:
  SCV_ENUM_CTOR(packet_types_t) {
    SCV_ENUM(UNICAST);
    SCV_ENUM(MULTICAST);
  };
};

SCV_EXTENSIONS(packet) {
 public:
    scv_extensions< packet_types_t > ptype;
    scv_extensions< sc_uint<8> > source;
    scv_extensions< sc_uint<8> > destination;

  SCV_EXTENSIONS_CTOR(packet) {
    SCV_FIELD(ptype);
    SCV_FIELD(source);
    SCV_FIELD(destination);
  }

};

#endif // PKT_EXT_H


