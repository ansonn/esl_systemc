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

//enum for one hot coding
struct state_t {
  enum onehot_t {
    STATE_0 = 0, STATE_1 = 1, STATE_2 = 2, STATE_3 = 4,
    STATE_4 = 8, STATE_5 = 16, STATE_6 = 32 };

  state_t::onehot_t state;
  int      delay;
  unsigned data;
};

