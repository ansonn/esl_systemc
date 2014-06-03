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
 packet_constraints.h --
  
 Define three constraint class qualis_packet, default_packet and 
 intermediate_packet for later use by test writers

 Define an inline method 'constrain_packet' used by test writers to constrain
 a scv_smart_ptr<packet> object defined in the test.
 *****************************************************************************/

#ifndef PACKET_CONSTRAINTS_H
#define PACKET_CONSTRAINTS_H

#include "scv.h"
#include "packet.h"

enum packet_constraint_type {QUALIS_PACKET, DEFAULT_PACKET, INTERMEDIATE_PACKET};

class qualis_packet : virtual public scv_constraint_base {
public:
  scv_smart_ptr<packet> sp;

  SCV_CONSTRAINT_CTOR(qualis_packet) {
    sp->ptype.keep_only(UNICAST);
    SCV_SOFT_CONSTRAINT(sp->source() <= 0xf);
  }
};

class default_packet : virtual public scv_constraint_base {
public:
  scv_smart_ptr<packet> sp;
public:
  SCV_CONSTRAINT_CTOR(default_packet) {
    SCV_CONSTRAINT(sp->source() <= 0x3);
    SCV_CONSTRAINT(sp->destination() <= 0x3);
    SCV_SOFT_CONSTRAINT(sp->source() != sp->destination());
  } 
};

class intermediate_packet : virtual public scv_constraint_base {
public:
  scv_smart_ptr<packet> sp;
public:
  SCV_CONSTRAINT_CTOR(intermediate_packet) {
    SCV_CONSTRAINT(sp->source() <= 0xf);
    SCV_CONSTRAINT(sp->destination() <= 0xf);
    SCV_SOFT_CONSTRAINT(sp->source() >= sp->destination());
  }
};

inline void constrain_packet(scv_smart_ptr<packet>& sp, packet_constraint_type t)
{
  switch(t) {
  case QUALIS_PACKET: {
    qualis_packet qp("qp");
    sp->use_constraint(qp.sp);
    break;
  }
  case DEFAULT_PACKET: {
    default_packet dp("dp");
    sp->use_constraint(dp.sp);
    break;
  }
  case INTERMEDIATE_PACKET: {
    intermediate_packet ip("ip");
    sp->use_constraint(ip.sp);
    break;
  }
  default:
    break;
  }
}

#endif
