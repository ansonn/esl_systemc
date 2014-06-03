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

  scv_util.h --
  The public interface for various small facilities.

  Original Authors (Cadence Design Systems, Inc):
  Norris Ip, Dean Shea, John Rose, Jasvinder Singh, William Paulsen,
  John Pierce, Rachida Kebichi, Ted Elkind, David Bailey
  2002-09-23

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date:
  Description of Modification:

 *****************************************************************************/

#ifndef SCV_UTIL_H
#define SCV_UTIL_H

#include <iostream>
#include <string>

#include "systemc"


int _scv_make_unique_id(const std::string& name, const std::string& kind);
const std::string _scv_make_unique_name(const std::string& name, int id);


//
// process name
//

typedef const char *(_scv_process_name_server_t)(sc_core::sc_process_handle proc_p);
extern _scv_process_name_server_t *_scv_process_name_server;
void _scv_set_process_name_server(_scv_process_name_server_t *);
const char *_scv_get_process_name(const sc_core::sc_process_handle proc_p);

const char *scv_get_process_name(sc_core::sc_process_handle proc_p);


//
// scv_startup
//

#define SCV_STARTUP() { \
  static bool dummy = scv_startup(); \
  if (0) cout << dummy << endl; \
}

bool scv_startup();

//
// scv_out
//

extern std::ostream *_scv_out_p;
#define scv_out (*_scv_out_p)

// Keep for backward compatibility
//
// tb_status (for regression convenience)
//
#define tb_status cout
#define TBS_INFO "%%TB_STATUS_INFO: "
#define TBS_WARNING "%%TB_STATUS_WARNING: "
#define TBS_ERROR "%%TB_STATUS_ERROR: "
#define TBS_SUCCESS "%%TB_STATUS_SUCCESS: "
#define TBS_FAIL "%%TB_STATUS_FAIL: "

#endif
