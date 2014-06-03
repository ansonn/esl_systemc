//  -*- C++ -*- <this line is for emacs to recognize it as C++ code>
/*****************************************************************************

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

  scv_report.cpp -- A trace/debug facility for the SystemC Verification
  Standard library

  Original Authors (Cadence Design Systems, Inc):
  Norris Ip, Dean Shea, John Rose, Jasvinder Singh, William Paulsen,
  John Pierce, Rachida Kebichi, Ted Elkind, David Bailey
  2002-09-23

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Torsten Maehne
                               Universite Pierre et Marie Curie, 2013-12-02
  Description of Modification: Drop scv_report and its implementation details
                               in favor of sc_core::sc_report.

 *****************************************************************************/

//
// This must be included before any system headers.
//
#include "scv.h"

#include <cstdarg>
#include <string>
#include <map>


//
// Implementation of _scv_message
//


#define _SCV_DEFERR(code, number, string, severity, stack_action) \
  _scv_message_desc *_scv_message::code##_base = 0; \
  _scv_message_desc **_scv_message::code = &_scv_message::code##_base;
#include "scv/scv_messages.h"
#undef _SCV_DEFERR


void _scv_message::message(_scv_message_desc **desc_pp, ...)
{
  // make sure desc is defined
  _scv_message::setup();

  _scv_message_desc *desc_p = *desc_pp;

  const char *tag = desc_p->get_tag();
  const char *format = desc_p->get_format();
  scv_severity severity = desc_p->get_severity();
  scv_actions actions = desc_p->get_actions();

  scv_actions hold = scv_report_handler::force(0);
  scv_report_handler::force(hold|actions);

  static char formattedMessageString[20000];
  std::va_list ap;
  va_start(ap,desc_pp);
  vsprintf(formattedMessageString,format,ap);
  va_end(ap);

  scv_report_handler::report(severity,tag,formattedMessageString,"unknown",0);

  scv_report_handler::force(hold);
}

void _scv_message::setup()
{
  static bool first_time = true;
  if ( first_time ) first_time = false;
  else return;
#define _SCV_DEFERR(code, number, string, severity, stack_action) \
  code##_base = new _scv_message_desc(#code,string,xlat_severity(severity),SCV_DO_NOTHING);
#include "scv/scv_messages.h"
#undef _SCV_DEFERR
  scv_report_handler::set_actions(SCV_ERROR,SCV_LOG|SCV_DISPLAY|SCV_CACHE_REPORT);
  scv_report_handler::set_actions(SCV_FATAL,SCV_LOG|SCV_DISPLAY|SCV_CACHE_REPORT);
}

scv_severity _scv_message::xlat_severity(severity_level severity)
{
  switch ( severity ) {
    case INFO    : return SCV_INFO;
    case WARNING : return SCV_WARNING;
    case ERROR   : return SCV_ERROR;
    default	 : return SCV_FATAL;
  }
}
