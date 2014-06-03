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

  scv_util.cpp -- The implementation of various small facilities.

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

#include "scv/scv_util.h"
#include "scv/_scv_associative_array.h"
#include "scv/scv_report.h"

/* ************************************************************************** */

// For cdsIdent:

#include "scv/scv_kit_date.h"
#include "scv/scv_ver.h"

#ifndef SCV_VERSION
#define SCV_VERSION "<SCV_VERSION undefined>"
#endif

#ifndef SCV_KIT_DATE
#define SCV_KIT_DATE "<SCV_KIT_DATE undefined>"
#endif


static struct {
  const char* strP;
  const char** strPP;
} scvVersionIdent = { "@(#)$CDS: libscv.so " SCV_VERSION " " SCV_KIT_DATE " $",
		&scvVersionIdent.strP };

/* ************************************************************************** */


//
// scv_startup
//


bool _scv_startup_called = false;

extern void scv_constraint_startup();

bool scv_startup()
{
  static bool first = true;
  if ( first ) {
    scv_constraint_startup();
    first = false;
  }
  return true;
}


//
// making unique names
//


int _scv_make_unique_id(const std::string& name, const std::string& kind)
{
  typedef _scv_associative_array<std::string,int> xref;
  static xref table("NameList",0);
  static const std::string delim = ":::";
  return table[kind+delim+name]++;
}

const std::string _scv_make_unique_name(const std::string& name, int id)
{
  static char *image = 0;
  static int len = 0;
  if ( id == 0 ) return name;
  int tmp = strlen(name.c_str()) + 36;
  if ( tmp > len ) {
    delete [] image;
    image = new char[tmp];
    len = tmp;
  }
  sprintf(image,"%s(%d)",name.c_str(),id);
  return image;
}


//
// Determine process name
//


_scv_process_name_server_t *_scv_process_name_server = 0;

void _scv_set_process_name_server(_scv_process_name_server_t *server)
{ _scv_process_name_server = server; }

const char *_scv_get_process_name(const sc_core::sc_process_handle proc_p)
// Enhance later to return unique name that's as stable as
// possible despite changes in order of execution.
{ return proc_p.name(); }

const char *scv_get_process_name(sc_core::sc_process_handle proc_p)
{
  if ( proc_p.valid() ) return "<main>";
  if ( _scv_process_name_server ) {
    return _scv_process_name_server(proc_p);
  }
  return _scv_get_process_name(proc_p);
}


//
// Class and associated methods for scv_out
//

class _scv_out_buf_t : public std::streambuf {
 public:
  int sync();
  int overflow(int ch); // Called with just one character
  int flush();
};

_scv_out_buf_t *_scv_out_buf_p = new _scv_out_buf_t;
std::ostream *_scv_out_p = new std::ostream(_scv_out_buf_p);
static int _scv_out_buffer_index = 0;
static char _scv_out_buffer[3000];
static bool _add_scv_prefix = true;
static char *_scv_prefix = getenv("SCV_REG");

int _scv_out_buf_t::sync() {
  if (_scv_out_buffer_index == 0) {
    return 0;
  }
  _scv_out_buffer[_scv_out_buffer_index] = '\0';
  if (_scv_prefix == NULL || strlen(_scv_prefix) == 0) {
    _scv_out_buffer_index = 0;
    std::cout << _scv_out_buffer;
    return 0;
  }
  char scv_prefix[10];
  if (_add_scv_prefix) {
    strcpy(scv_prefix, _scv_prefix);
  } else {
    scv_prefix[0] = '\0';
  }
  char scv_spare_buffer[3000];
  char *begin_line = &_scv_out_buffer[0];
  char *spare_buffer = &scv_spare_buffer[0];
  char *chr;
  for (chr = &_scv_out_buffer[0]; *chr != '\0'; chr++) {
    if (*chr == '\n') {
      strcpy(spare_buffer, scv_prefix);
      spare_buffer += strlen(scv_prefix);
      while (begin_line != chr) {
        *spare_buffer++ = *begin_line++;
      }
    }
  }
  if (begin_line < chr) {
    while (begin_line <= chr) {
      *spare_buffer++ = *begin_line++;
    }
  }
  if (*(chr-1) == '\n') {
    *(spare_buffer-2) = '\0';
  }
  strcpy(_scv_out_buffer, scv_spare_buffer);
  _scv_out_buffer_index = 0;
  std::cout << _scv_out_buffer << std::endl;
  return 0;
}

int _scv_out_buf_t::flush() {
  _scv_out_buf_p->sync();
  return 0;
}

int _scv_out_buf_t::overflow(int ch) {
  // Called with one char
  char c;
  c = (char) ch;
  _scv_out_buffer[_scv_out_buffer_index] = c;
  _scv_out_buffer_index++;
  if ( (_scv_out_buffer_index > 2000) || (c == '\n') ) {
    _scv_out_buf_p->sync();
  }
  return ch;
}

//
// Data structure initialization

int _scv_data_structure::_debug = -1;

const char *_scv_data_structure::_kind = "_scv_data_structure";

void scv_object_if::set_debug_level(const char *facility, int level)
{  scv_debug::set_level(facility,level); }
