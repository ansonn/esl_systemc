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
#include "packet_ext.h"

// nbcode "class" start
//create a basic constraint for the packet generator
struct packet_base_constraint : public scv_constraint_base {
  //create a packet object
  scv_smart_ptr<packet_t> packet;

  //put the base constraints on the packet variables
  SCV_CONSTRAINT_CTOR(packet_base_constraint) {
    SCV_CONSTRAINT ( packet->msg_length() < 80 );
    SCV_CONSTRAINT ( packet->msg_length() > 2 );
    SCV_CONSTRAINT ( packet->src_addr() != packet->dest_addr() );

    for (int i=0; i<1024; ++i) {
      //make the msg alpha numeric
      packet->msg[i] = '\0';

      //you can use keep_only/keep_out only if the element you
      //are constraining is NOT involved in SCV_CONSTRAINTS in
      //any way. In this case, msg is independent of other things.
      packet->msg[i].keep_only(' ', 'z');  //ascii space to z
      packet->msg[i].keep_out('!', '/');   //punctuation characters
      packet->msg[i].keep_out(':', '@');   //more special characters
      packet->msg[i].keep_out('[', '`');   //more special characters
    }
  }
};
// nbcode "class" end
