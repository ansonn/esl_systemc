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

// nbcode "packet" start
struct packet_t {
  sc_uint<32> src_addr;
  sc_uint<32> dest_addr;
  sc_uint<16> msg_length;
  char        msg[1024];
};
// nbcode "packet" end

// nbcode "out" start
//define an ostream for a packet object
ostream& operator<<(ostream& os, const packet_t& p) {
  os << "{\n"
     << "   src_addr: " << p.src_addr << "\n"
     << "   dest_addr: " << p.dest_addr << "\n"
     << "   msg_length: " << p.msg_length << "\n"
     << "   msg {\n"
     << "     " << p.msg  << "\n"
     << "   }\n}" << endl;
  return os;
}
// nbcode "out" end

