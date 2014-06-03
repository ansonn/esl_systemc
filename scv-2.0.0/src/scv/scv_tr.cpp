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

  scv_tr.cpp -- The implementation of the transaction recording facility in
  scv_tr.h

  Original Authors (Cadence Design Systems, Inc):
  Norris Ip, Dean Shea, John Rose, Jasvinder Singh, William Paulsen,
  John Pierce, Rachida Kebichi, Ted Elkind, David Bailey
  2002-09-23

 *****************************************************************************/

/*****************************************************************************

  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
  changes you are making here.

      Name, Affiliation, Date: Torsten Maehne,
                               Universite Pierre et Marie Curie, 2013-04-30
  Description of Modification: Fix memory leak caused by not free()-ing
                               C-strings duplicated using strdup() in the
                               internal classes _scv_tr_db_core,
                               _scv_tr_stream_core, and _scv_tr_generator_core
                               by changing the type of the concerned
                               member variables from char* to string.
                               Remove the now unused internal
                               static scv_tr_strdup() function, which is anyway
                               not exported by the linker.

 *****************************************************************************/

#include <list>
#include <map>
#include <string>

#include "scv.h"
#include "scv/scv_debug.h"
#include "scv/scv_introspection.h"
#include "scv/scv_tr.h"

// ----------------------------------------------------------------------------

//#define scv_tr_TRACE

// ----------------------------------------------------------------------------

static void scv_tr_null_scv_tr_db_message()
{
  static bool message_given = false;

  if (message_given == false) {
    message_given = true;

   _scv_message::message(
                _scv_message::TRANSACTION_RECORDING_INTERNAL,
                "The scv_tr_db argument to a scv_tr method is NULL");
  }
}

// ----------------------------------------------------------------------------

typedef void _scv_tr_void_function_t();

class _scv_tr_callback_item_t {
 public:

  _scv_tr_callback_item_t()
  {
    id_counter++;
    this->id = id_counter;
  }

  static int id_counter;
  int id;
  void* callback_fp;
  void* user_data_p;
};

int _scv_tr_callback_item_t::id_counter = 0;

typedef std::list<_scv_tr_callback_item_t*> _scv_tr_callback_list;

// ----------------------------------------------------------------------------

class _scv_tr_db_core {
 public:

  _scv_tr_db_core(
	const char* recording_file_name,
	const sc_time_unit& _time_unit);

  ~_scv_tr_db_core() {}

  static scv_tr_db* default_scv_tr_db_p;
  scv_tr_db* my_scv_tr_db_p;
  sc_time_unit my_sc_time_unit;
  std::string my_name;
  bool state;
  static int debug;

  uint64 global_id;  // Global counter of objects in this scv_tr_db
  uint64 global_transaction_id;

  // A list of callbacks registered for this class:
  //
  static _scv_tr_callback_list* callback_list_p;

  // A map of relation string names, by handle:
  //
  scv_tr_relation_handle_t relation_handle_counter;
  std::map<scv_tr_relation_handle_t, std::string> relation_by_handle_map;
  std::map<std::string, scv_tr_relation_handle_t> relation_by_name_map;
};

int _scv_tr_db_core::debug = -1;
scv_tr_db* _scv_tr_db_core::default_scv_tr_db_p = NULL;
_scv_tr_callback_list* _scv_tr_db_core::callback_list_p = NULL;

// ----------------------------------------------------------------------------

_scv_tr_db_core::_scv_tr_db_core(
        const char* recording_file_name,
        const sc_time_unit& _time_unit)
{
  if (recording_file_name) {
    this->my_name = recording_file_name;
  }
  this->state = true;
  this->my_sc_time_unit = _time_unit;
  this->global_id = 1;
  this->global_transaction_id = 1;
  this->relation_handle_counter = 0;
}

// ----------------------------------------------------------------------------

class _scv_tr_stream_core {
  friend class scv_tr_stream;
 public:
  scv_tr_stream* my_scv_tr_stream_p;
  std::string my_name;
  std::string my_stream_kind_name;
  _scv_tr_db_core* my_scv_tr_db_core_p;
  static int debug;

  uint64 my_id;

  static _scv_tr_callback_list* callback_list_p;

  _scv_tr_stream_core(
		_scv_tr_db_core* _scv_tr_db_core_p,
		scv_tr_stream* scv_tr_stream_p,
		const char* name,
                const char* _stream_kind_name)
  {
    this->my_scv_tr_db_core_p = _scv_tr_db_core_p;
    this->my_scv_tr_stream_p = scv_tr_stream_p;
    if (name) {
      this->my_name = name;
    }
    if (_stream_kind_name) {
      this->my_stream_kind_name = _stream_kind_name;
    }
    this->my_id = this->my_scv_tr_db_core_p->global_id++;
  }

};

int _scv_tr_stream_core::debug = -1;
_scv_tr_callback_list* _scv_tr_stream_core::callback_list_p = NULL;

// ----------------------------------------------------------------------------

class _scv_tr_generator_core {
  friend class scv_tr_stream;
 public:
  uint64 my_id;
  std::string my_name;
  _scv_tr_stream_core* my_scv_tr_stream_core_p;
  scv_tr_generator_base* my_scv_tr_generator_base_p;
  static _scv_tr_callback_list* callback_list_p;
  const scv_extensions_if* begin_exts_p;
  const scv_extensions_if* end_exts_p;
  static int debug;
  std::string my_begin_attribute_name; 
  std::string my_end_attribute_name;

  _scv_tr_generator_core(
		_scv_tr_stream_core* scv_tr_stream_core_p,
		scv_tr_generator_base* scv_tr_generator_base_p,
		const char* name,
		const char* begin_attribute_name,
		const char* end_attribute_name);
};

int _scv_tr_generator_core::debug = -1;
_scv_tr_callback_list* _scv_tr_generator_core::callback_list_p = NULL;

// ----------------------------------------------------------------------------

_scv_tr_generator_core::_scv_tr_generator_core(
        _scv_tr_stream_core* scv_tr_stream_core_p,
        scv_tr_generator_base* scv_tr_generator_base_p,
        const char* name,
        const char* begin_attribute_name,
        const char* end_attribute_name)
{
#ifdef scv_tr_TRACE
  cout << "Entering _scv_tr_generator_core ctor\n";
#endif

  if ( (scv_tr_stream_core_p == NULL) ||
                (scv_tr_generator_base_p == NULL) ||
                (scv_tr_stream_core_p->my_scv_tr_db_core_p == NULL) ) {

    scv_tr_null_scv_tr_db_message();

    this->my_scv_tr_stream_core_p = NULL;
    this->my_scv_tr_generator_base_p = NULL;
    this->begin_exts_p = NULL;
    this->end_exts_p = NULL;
    this->my_id = 1;
    return;
  }

  this->my_scv_tr_stream_core_p = scv_tr_stream_core_p;
  this->my_scv_tr_generator_base_p = scv_tr_generator_base_p;
  if (name) { this->my_name = name; }
  this->begin_exts_p = NULL;
  this->end_exts_p = NULL;
  this->my_id = scv_tr_stream_core_p->my_scv_tr_db_core_p->global_id++;

  if (begin_attribute_name) {
    this->my_begin_attribute_name = begin_attribute_name; 
  }
  if (end_attribute_name) {
    this->my_end_attribute_name = end_attribute_name;
  }

#ifdef scv_tr_TRACE
  cout << "Leaving _scv_tr_generator_core ctor\n";
#endif
}

// ----------------------------------------------------------------------------

class _scv_tr_handle_core {
 public:
  sc_time begin_sc_time;
  sc_time end_sc_time;

  bool is_valid;
  bool is_active;
  _scv_tr_generator_core* my_scv_tr_generator_core_p;
  static int debug;

  uint64 my_id;

  const scv_extensions_if* begin_exts_p;
  const scv_extensions_if* end_exts_p;

  std::string my_name;

  int ref_count;

  static _scv_tr_callback_list* callback_list_p;
  static _scv_tr_callback_list* callback_relation_list_p;
  static _scv_tr_callback_list* callback_record_attribute_list_p;

  scv_tr_relation_handle_t immediate_relation_handle;
  const scv_tr_handle* immediate_other_transaction_handle_p;

  _scv_tr_handle_core();
  ~_scv_tr_handle_core();
};

int _scv_tr_handle_core::debug = -1;
_scv_tr_callback_list* _scv_tr_handle_core::callback_list_p = NULL;
_scv_tr_callback_list* _scv_tr_handle_core::callback_relation_list_p = NULL;
_scv_tr_callback_list* _scv_tr_handle_core::callback_record_attribute_list_p =
						NULL;

// ----------------------------------------------------------------------------

_scv_tr_handle_core::_scv_tr_handle_core() :
	begin_sc_time(SC_ZERO_TIME), end_sc_time(SC_ZERO_TIME)
{
#ifdef scv_tr_TRACE
  cout << "In _scv_tr_handle_core ctor\n";
#endif
  this->is_valid = false;
  this->is_active = false;
  this->my_scv_tr_generator_core_p = NULL;
  this->my_id = 0;
  this->ref_count = 1;
}

_scv_tr_handle_core::~_scv_tr_handle_core()
{
#ifdef scv_tr_TRACE
  cout << "In _scv_tr_handle_core dtor\n";
#endif
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template<
	class object_class_parameter_t,
	typename callback_reason_parameter_t,
	typename callback_function_parameter_t
	>
static void process_callbacks(
	const object_class_parameter_t& obj,
	_scv_tr_callback_list* callback_list_p,
	callback_reason_parameter_t callback_reason)
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr process_callbacks\n";
#endif

  // This is a template function that processes callbacks for different
  // scv_tr_... classes.

  if (callback_list_p != NULL) {
    bool need_cleanup = false;

    _scv_tr_callback_list::iterator i = callback_list_p->begin();

    for ( ; i != callback_list_p->end(); i++) {
      if ( (*i)->callback_fp == NULL) {
        // This callback has been removed, so we need to clean up after
        // this iterator.  We can't remove it here because that would
        // corrupt the iterator.
        need_cleanup = true;
      } else {

        if (obj.get_debug() >= 0) {
          cout << "scv_tr debug: process_callbacks" << endl;
        }

        callback_function_parameter_t* my_callback_fp =
		(callback_function_parameter_t*)((*i)->callback_fp);

#ifdef scv_tr_TRACE
  cout << "  calling my_callback_fp = " << (long)my_callback_fp << endl;
  cout << "  obj.get_name() = " << obj.get_name() << endl;
#endif

	my_callback_fp(
		obj,
		callback_reason,
		(*i)->user_data_p);
      }
    }

    while (need_cleanup == true) {
      // Iterate on the list looking for items that have been marked as
      // removed, and then remove them.

      i = callback_list_p->begin();

      for ( ; i != callback_list_p->end(); i++) {
        if ( (*i)->callback_fp == NULL) {
          delete *i;
          callback_list_p->erase(i);
          goto continue_label;
        }
      }
      // If you get here, then there are no more items to remove.
      need_cleanup = false;

      continue_label: ;
    }
  }

#ifdef scv_tr_TRACE
  cout << "Leaving process_callbacks\n";
#endif
}

// ----------------------------------------------------------------------------

static void process_record_attribute_callbacks(
        const scv_tr_handle& obj,
        const char* attribute_name,
        _scv_tr_callback_list* callback_list_p,
	const scv_extensions_if* my_exts_p)
{
  // This function processes record_attribute callbacks.

  if (callback_list_p != NULL) {
    bool need_cleanup = false;

    _scv_tr_callback_list::iterator i = callback_list_p->begin();

    for ( ; i != callback_list_p->end(); i++) {
      if ( (*i)->callback_fp == NULL) {
        // This callback has been removed, so we need to clean up after
        // this iterator.  We can't remove it here because that would
        // corrupt the iterator.
        need_cleanup = true;
      } else {
        scv_tr_handle::callback_record_attribute_function* my_callback_fp =
                (scv_tr_handle::callback_record_attribute_function*)
			((*i)->callback_fp);
        my_callback_fp(
                obj,
		attribute_name,
		my_exts_p,
                (*i)->user_data_p);
      }
    }

    while (need_cleanup == true) {
      // Iterate on the list looking for items that have been marked as
      // removed, and then remove them.

      i = callback_list_p->begin();

      for ( ; i != callback_list_p->end(); i++) {
        if ( (*i)->callback_fp == NULL) {
          delete *i;
          callback_list_p->erase(i);
          goto continue_label;
        }
      }
      // If you get here, then there are no more items to remove.
      need_cleanup = false;

      continue_label: ;
    }
  }
}

// ----------------------------------------------------------------------------

static void process_relation_callbacks(
        const scv_tr_handle& obj,
        const scv_tr_handle& obj_2,
        _scv_tr_callback_list* callback_list_p,
        scv_tr_relation_handle_t relation_handle)
{
  // This is a function that processes callbacks for scv_tr_handle:add_relation

  if (callback_list_p != NULL) {
    bool need_cleanup = false;

    _scv_tr_callback_list::iterator i = callback_list_p->begin();

    for ( ; i != callback_list_p->end(); i++) {
      if ( (*i)->callback_fp == NULL) {
        // This callback has been removed, so we need to clean up after
        // this iterator.  We can't remove it here because that would
        // corrupt the iterator.
        need_cleanup = true;
      } else {
        scv_tr_handle::callback_relation_function* my_callback_fp =
                (scv_tr_handle::callback_relation_function*)((*i)->callback_fp);

        my_callback_fp(
                obj,
		obj_2,
                (*i)->user_data_p,
		relation_handle);
      }
    }

    while (need_cleanup == true) {
      // Iterate on the list looking for items that have been marked as
      // removed, and then remove them.

      i = callback_list_p->begin();

      for ( ; i != callback_list_p->end(); i++) {
        if ( (*i)->callback_fp == NULL) {
          delete *i;
          callback_list_p->erase(i);
          goto continue_label;
        }
      }
      // If you get here, then there are no more items to remove.
      need_cleanup = false;

      continue_label: ;
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

const char *scv_tr_db::_kind = "scv_tr_db";

scv_tr_db::scv_tr_db(
	const char* recording_file_name,
	const sc_time_unit& _time_unit)
{
  if (this->get_debug() >= 0) {
    cout << "scv_tr debug: entering scv_tr_db ctor, name = " <<
				recording_file_name << endl;
  }

  this->_scv_tr_db_core_p = new _scv_tr_db_core(
		recording_file_name,
		_time_unit);

  if (_scv_tr_db_core::default_scv_tr_db_p == NULL) {
   _scv_tr_db_core::default_scv_tr_db_p = this;
  }

  this->_scv_tr_db_core_p->my_scv_tr_db_p = this;

  // Make any need callbacks:

  process_callbacks<
		scv_tr_db,
		scv_tr_db::callback_reason,
		scv_tr_db::callback_function> (
		*this,
		_scv_tr_db_core::callback_list_p,
		scv_tr_db::CREATE);

  if (this->get_debug() >= 0) {
    cout << "scv_tr debug: leaving scv_tr_db ctor, name = " <<
                        recording_file_name << endl;
  }
}

// ----------------------------------------------------------------------------

scv_tr_db::~scv_tr_db()
{
  int d = this->get_debug();

  if (d >= 0) {
    cout << "scv_tr debug: entering scv_tr_db dtor, name = " <<
	this->get_name() << endl;
  }

  if (_scv_tr_db_core::default_scv_tr_db_p == this) {
    _scv_tr_db_core::default_scv_tr_db_p = NULL;
  }

  process_callbacks<
                scv_tr_db,
                scv_tr_db::callback_reason,
                scv_tr_db::callback_function> (
                *this,
                _scv_tr_db_core::callback_list_p,
                scv_tr_db::DELETE);

  delete this->_scv_tr_db_core_p;

  if (d >= 0) {
    cout << "scv_tr debug: leaving scv_tr_db dtor" << endl;
  }
}

// ----------------------------------------------------------------------------

scv_tr_db::callback_h scv_tr_db::register_class_cb(
        scv_tr_db::callback_function* cbf,
        void* user_data_p)
{
  if (_scv_tr_db_core::callback_list_p == NULL) {
    _scv_tr_db_core::callback_list_p = new _scv_tr_callback_list;
  }

  _scv_tr_callback_item_t* my_item_p = new _scv_tr_callback_item_t;

  my_item_p->callback_fp = (void*) cbf;
  my_item_p->user_data_p = (void*) user_data_p;

  _scv_tr_db_core::callback_list_p->push_back(my_item_p);

  return my_item_p->id;
}

// ----------------------------------------------------------------------------

void scv_tr_db::set_default_db(scv_tr_db* db)
{
  _scv_tr_db_core::default_scv_tr_db_p = db;
}

// ----------------------------------------------------------------------------

scv_tr_db* scv_tr_db::get_default_db()
{
  return _scv_tr_db_core::default_scv_tr_db_p;
}

// ----------------------------------------------------------------------------

void scv_tr_db::set_recording(bool b) const
{
  if (this->_scv_tr_db_core_p == NULL) return;

  this->_scv_tr_db_core_p->state = b; // true is to resume recording
}

// ----------------------------------------------------------------------------

bool scv_tr_db::get_recording() const
{
  if (this->_scv_tr_db_core_p == NULL) return false;

  return this->_scv_tr_db_core_p->state;
}

// ----------------------------------------------------------------------------

void scv_tr_db::print(ostream& o, int details, int indent) const
{
  o << "scv_tr_db: " << this->get_name() << endl;
}

// ----------------------------------------------------------------------------

void scv_tr_db::show(int details, int indent) const
{
  this->print(scv_out, details, indent);
}

// ----------------------------------------------------------------------------

void scv_tr_db::set_debug(int d)
{
  scv_debug::set_facility_level(scv_debug::RECORDING, d);

  _scv_tr_db_core::debug = d;
}

// ----------------------------------------------------------------------------

int scv_tr_db::get_debug()
{
  return _scv_tr_db_core::debug;
}

// ----------------------------------------------------------------------------

const char *scv_tr_db::get_name() const
{
  if (this->_scv_tr_db_core_p == NULL) {
    static std::string tmp_name = "<anonymous>";
    return tmp_name.c_str();
  }

  static std::string tmp_my_name;
  tmp_my_name = this->_scv_tr_db_core_p->my_name;
  return tmp_my_name.c_str();
}

// ----------------------------------------------------------------------------

void scv_tr_db::remove_callback(callback_h h)
{
  _scv_tr_callback_list* my_cb_list_p = _scv_tr_db_core::callback_list_p;

  if (my_cb_list_p == NULL) return;

  _scv_tr_callback_list::iterator i = my_cb_list_p->begin();

  for ( ; i != my_cb_list_p->end(); i++) {
    if ((*i)->id == h) {
      // The entry gets deleted the next time callbacks get processed -
      // you can't delete here because you might currently be in the middle of
      // a callback, and that would screw up the list if the item were deleted.
      (*i)->callback_fp = NULL;
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

const char *scv_tr_stream::_kind = "scv_tr_stream";

scv_tr_stream::scv_tr_stream(
        const char* full_stream_name,
        const char* kind_name,
        scv_tr_db* scv_tr_db_p)
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_stream ctor\n";
#endif

  if (scv_tr_db_p == NULL) {
    scv_tr_null_scv_tr_db_message();
    this->_scv_tr_stream_core_p = NULL;
    return;
  }

  this->_scv_tr_stream_core_p = new _scv_tr_stream_core(
		scv_tr_db_p->_scv_tr_db_core_p,
		this,
		full_stream_name,
		kind_name
		);

  process_callbacks<
		scv_tr_stream,
		scv_tr_stream::callback_reason,
		scv_tr_stream::callback_function> (
		*this,
		_scv_tr_stream_core::callback_list_p,
		scv_tr_stream::CREATE);
}

// ----------------------------------------------------------------------------

scv_tr_stream::~scv_tr_stream()
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_stream dtor\n";
#endif

  process_callbacks<
                scv_tr_stream,
                scv_tr_stream::callback_reason,
                scv_tr_stream::callback_function> (
                *this,
                _scv_tr_stream_core::callback_list_p,
                scv_tr_stream::DELETE);

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_stream dtor\n";
#endif
}

// ----------------------------------------------------------------------------

scv_tr_stream::callback_h scv_tr_stream::register_class_cb(
        scv_tr_stream::callback_function* cbf,
        void* user_data_p)
{
  if (_scv_tr_stream_core::callback_list_p == NULL) {
    _scv_tr_stream_core::callback_list_p = new _scv_tr_callback_list;
  }

  _scv_tr_callback_item_t* my_item_p = new _scv_tr_callback_item_t;

  my_item_p->callback_fp = (void*) cbf;
  my_item_p->user_data_p = (void*) user_data_p;

  _scv_tr_stream_core::callback_list_p->push_back(my_item_p);

  return my_item_p->id;
}

// ----------------------------------------------------------------------------

void scv_tr_stream::print(ostream& o, int details, int indent) const
{
  o << "scv_tr_stream:" << this->get_name() << endl;
}

// ----------------------------------------------------------------------------

void scv_tr_stream::show(int details, int indent) const
{
  this->print(scv_out, details, indent);
}

// ----------------------------------------------------------------------------

void scv_tr_stream::set_debug(int i) 
{
  _scv_tr_stream_core::debug = i;
}

// ----------------------------------------------------------------------------

int scv_tr_stream::get_debug()
{
  return _scv_tr_stream_core::debug;
}

// ----------------------------------------------------------------------------

const char *scv_tr_stream::get_name() const
{
  if (this->_scv_tr_stream_core_p == NULL) {
    static std::string tmp_name = "<anonymous>";
    return tmp_name.c_str();
  }

  static std::string tmp_my_name;
  tmp_my_name = this->_scv_tr_stream_core_p->my_name;
  return tmp_my_name.c_str();
}

// ----------------------------------------------------------------------------

uint64 scv_tr_stream::get_id() const
{
  if (this->_scv_tr_stream_core_p == NULL) return 0;

  return this->_scv_tr_stream_core_p->my_id;
}

// ----------------------------------------------------------------------------

const char* scv_tr_stream::get_stream_kind() const
{
  if (this->_scv_tr_stream_core_p == NULL) return NULL;
  return this->_scv_tr_stream_core_p->my_stream_kind_name.c_str();
}

// ----------------------------------------------------------------------------

void scv_tr_stream::remove_callback(callback_h h)
{
  _scv_tr_callback_list* my_cb_list_p = _scv_tr_stream_core::callback_list_p;

  if (my_cb_list_p == NULL) return;

  _scv_tr_callback_list::iterator i = my_cb_list_p->begin();

  for ( ; i != my_cb_list_p->end(); i++) {
    if ((*i)->id == h) {
      // The entry gets deleted the next time callbacks get processed -
      // you can't delete here because you might currently be in the middle of
      // a callback, and that would screw up the list if the item were deleted.
      (*i)->callback_fp = NULL;
    }
  }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

const char *scv_tr_handle::_kind = "scv_tr_handle";

scv_tr_handle::scv_tr_handle()
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_handle ctor\n";
#endif

  this->_scv_tr_handle_core_p = NULL;

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_handle ctor\n";
#endif
}

// ----------------------------------------------------------------------------

scv_tr_handle::~scv_tr_handle()
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_handle dtor\n";
  cout << "  this->get_id() = " << this->get_id() << endl;
#endif

  if (this->_scv_tr_handle_core_p != NULL) {

#ifdef scv_tr_TRACE
  cout << "  this->_scv_tr_handle_core_p->ref_count = "
	<< _scv_tr_handle_core_p->ref_count << endl;
#endif

    if (this->_scv_tr_handle_core_p->ref_count == 1) {

      process_callbacks<
                scv_tr_handle,
                scv_tr_handle::callback_reason,
                scv_tr_handle::callback_function> (
                *this,
                _scv_tr_handle_core::callback_list_p,
                scv_tr_handle::DELETE);

      if (this->_scv_tr_handle_core_p->ref_count != 1) {
        _scv_message::message(
                _scv_message::TRANSACTION_RECORDING_INTERNAL_FATAL,
                "scv_tr_handle::~scv_tr_handle ref_count bad");

        return;
      }

#ifdef scv_tr_TRACE
  cout << "  delete _scv_tr_handle_core_p\n";
#endif

      delete this->_scv_tr_handle_core_p;
    } else {
      this->_scv_tr_handle_core_p->ref_count--;

#ifdef scv_tr_TRACE
  cout << "  new this->_scv_tr_handle_core_p->ref_count = "
        << _scv_tr_handle_core_p->ref_count << endl;
#endif
    }
  }

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_handle dtor\n";
#endif
}

// ----------------------------------------------------------------------------

scv_tr_handle &scv_tr_handle::operator=(
    const scv_tr_handle& other)
{
  // Copy assign

#ifdef scv_tr_TRACE
  cout << "scv_tr_handle operator=\n";
  cout << "  other.get_id() = " << other.get_id() << endl;
#endif

  if (this == &other) {
#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_handle operator=, this == &other\n";
#endif
    return *this;
  }

  if (this->_scv_tr_handle_core_p != NULL) {

#ifdef scv_tr_TRACE
  cout << "  this->get_id() = " << this->get_id() << endl;
  cout << "  this->_scv_tr_handle_core_p->ref_count = "
	<< this->_scv_tr_handle_core_p->ref_count << endl;
#endif

    this->_scv_tr_handle_core_p->ref_count--;

    if (this->_scv_tr_handle_core_p->ref_count == 0) {

      process_callbacks<
                scv_tr_handle,
                scv_tr_handle::callback_reason,
                scv_tr_handle::callback_function> (
                *this,
                _scv_tr_handle_core::callback_list_p,
                scv_tr_handle::DELETE);

#ifdef scv_tr_TRACE
  cout << "  In scv_tr_handle operator= after process_callbacks\n"
	<< "  this->_scv_tr_handle_core_p->ref_count = "
        << this->_scv_tr_handle_core_p->ref_count << endl;
#endif

      if (this->_scv_tr_handle_core_p->ref_count != 0) {
        _scv_message::message(
        _scv_message::TRANSACTION_RECORDING_INTERNAL_FATAL,
	"scv_tr_handle::operator= ref_count is bad after process_callbacks");

        return *this;
      }

      delete this->_scv_tr_handle_core_p;
    }
  }

  this->_scv_tr_handle_core_p = other._scv_tr_handle_core_p;

  if (this->_scv_tr_handle_core_p != NULL) {

    this->_scv_tr_handle_core_p->ref_count++;

#ifdef scv_tr_TRACE
  cout << "  In scv_tr_handle operator= after process_callbacks\n"
        << "  new other._scv_tr_handle_core_p->ref_count = "
        << other._scv_tr_handle_core_p->ref_count << endl;
#endif
  }

  return *this;
}

// ----------------------------------------------------------------------------

scv_tr_handle::scv_tr_handle(
    const scv_tr_handle& other)
{
  // Copy ctor

#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_handle copy ctor\n";
  cout << "  other.get_id() = " << other.get_id() << endl;
#endif

  this->_scv_tr_handle_core_p = other._scv_tr_handle_core_p;

  if (this->_scv_tr_handle_core_p) {

    this->_scv_tr_handle_core_p->ref_count++;

#ifdef scv_tr_TRACE
  cout << "  new this->_scv_tr_handle_core_p->ref_count = "
        << this->_scv_tr_handle_core_p->ref_count << endl;
#endif
  }

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_handle copy ctor\n";
#endif
}

// ----------------------------------------------------------------------------

void scv_tr_handle::_record_attribute(
	const char* attribute_name,
	const scv_extensions_if* my_exts_p)
{
  const char* tmp_attribute_name =
        attribute_name ? attribute_name : "<anonymous>";

  process_record_attribute_callbacks(
                *this,
                tmp_attribute_name,
                _scv_tr_handle_core::callback_record_attribute_list_p,
		my_exts_p);
}

// ----------------------------------------------------------------------------

scv_tr_handle::callback_h scv_tr_handle::register_class_cb(
        scv_tr_handle::callback_function* cbf,
	void* user_data_p)
{
  if (_scv_tr_handle_core::callback_list_p == NULL) {
    _scv_tr_handle_core::callback_list_p = new _scv_tr_callback_list;
  }

  _scv_tr_callback_item_t* my_item_p = new _scv_tr_callback_item_t;

  my_item_p->callback_fp = (void*) cbf;
  my_item_p->user_data_p = (void*) user_data_p;

  _scv_tr_handle_core::callback_list_p->push_back(my_item_p);

  return my_item_p->id;
}

// ----------------------------------------------------------------------------

scv_tr_handle::callback_h scv_tr_handle::register_record_attribute_cb(
        scv_tr_handle::callback_record_attribute_function* cbf,
        void* user_data_p)
{
  if (_scv_tr_handle_core::callback_record_attribute_list_p == NULL) {
    _scv_tr_handle_core::callback_record_attribute_list_p =
					new _scv_tr_callback_list;
  }

  _scv_tr_callback_item_t* my_item_p = new _scv_tr_callback_item_t;

  my_item_p->callback_fp = (void*) cbf;
  my_item_p->user_data_p = (void*) user_data_p;

  _scv_tr_handle_core::callback_record_attribute_list_p->push_back(my_item_p);

  return my_item_p->id;
}

// ----------------------------------------------------------------------------

scv_tr_handle::callback_h scv_tr_handle::register_relation_cb(
	scv_tr_handle::callback_relation_function* cbf,
        void* user_data_p)
{
  if (_scv_tr_handle_core::callback_relation_list_p == NULL) {
    _scv_tr_handle_core::callback_relation_list_p = new _scv_tr_callback_list;
  }

  _scv_tr_callback_item_t* my_item_p = new _scv_tr_callback_item_t;

  my_item_p->callback_fp = (void*) cbf;
  my_item_p->user_data_p = (void*) user_data_p;

  _scv_tr_handle_core::callback_relation_list_p->push_back(my_item_p);

  return my_item_p->id;
}

// ----------------------------------------------------------------------------

bool scv_tr_handle::is_valid() const
{
  if (this->_scv_tr_handle_core_p) {
    return this->_scv_tr_handle_core_p->is_valid;
  } else {
    return false;
  }
}

// ----------------------------------------------------------------------------

bool scv_tr_handle::is_active() const
{
  if (this->_scv_tr_handle_core_p) {
    return this->_scv_tr_handle_core_p->is_active;
  } else {
    return false;
  }
}

// ----------------------------------------------------------------------------

const scv_extensions_if* scv_tr_handle::get_begin_exts_p() const
{
  if (this->_scv_tr_handle_core_p == NULL) return NULL;

  return this->_scv_tr_handle_core_p->begin_exts_p;
}

// ----------------------------------------------------------------------------

const scv_extensions_if* scv_tr_handle::get_end_exts_p() const
{
  if (this->_scv_tr_handle_core_p == NULL) return NULL;

  return this->_scv_tr_handle_core_p->end_exts_p;
}

// ----------------------------------------------------------------------------

uint64 scv_tr_handle::get_id() const
{
  if (this->_scv_tr_handle_core_p) return this->_scv_tr_handle_core_p->my_id;
  else return 0;
}

// ----------------------------------------------------------------------------

void scv_tr_handle::print(ostream& o, int details, int indent) const
{
  o << "scv_tr_handle: " << this->get_name() << endl;
}

// ----------------------------------------------------------------------------

void scv_tr_handle::show(int details, int indent) const
{
  this->print(scv_out, details, indent);
}

// ----------------------------------------------------------------------------

void scv_tr_handle::set_debug(int i) 
{
  _scv_tr_handle_core::debug = i;
}

// ----------------------------------------------------------------------------

int scv_tr_handle::get_debug()
{
  return _scv_tr_handle_core::debug;
}

// ----------------------------------------------------------------------------

const char *scv_tr_handle::get_name() const
{
  if (this->_scv_tr_handle_core_p == NULL) {
    static std::string tmp_name = "<anonymous>";
    return tmp_name.c_str();
  }

  return this->_scv_tr_handle_core_p->my_name.c_str();
}

// ----------------------------------------------------------------------------

bool scv_tr_handle::add_relation(
        scv_tr_relation_handle_t relation_handle,
        const scv_tr_handle& other_transaction)
{
  process_relation_callbacks(
                *this,
		other_transaction,
                _scv_tr_handle_core::callback_relation_list_p,
                relation_handle);

  return true;
}

// ----------------------------------------------------------------------------

const scv_tr_handle* scv_tr_handle::get_immediate_related_transaction(
        scv_tr_relation_handle_t* relation_handle_p) const
{
  *relation_handle_p = this->_scv_tr_handle_core_p->immediate_relation_handle;
  return this->_scv_tr_handle_core_p->immediate_other_transaction_handle_p;
}

// ----------------------------------------------------------------------------

const sc_time& scv_tr_handle::get_begin_sc_time() const
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_handle::get_begin_sc_time\n";
#endif

  if (this->_scv_tr_handle_core_p == NULL) {

#ifdef scv_tr_TRACE
  cout << "  this->_scv_tr_handle_core_p is NULL\n";
#endif

    static sc_time tmp_sc_time;
    return tmp_sc_time;
  }

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_handle::get_begin_sc_time\n";
#endif

  return this->_scv_tr_handle_core_p->begin_sc_time;
}

// ----------------------------------------------------------------------------

const sc_time& scv_tr_handle::get_end_sc_time() const
{
  if (this->_scv_tr_handle_core_p == NULL) {
    static sc_time tmp_sc_time;
    return tmp_sc_time;
  }

  return this->_scv_tr_handle_core_p->end_sc_time;
}

// ----------------------------------------------------------------------------

const scv_tr_stream& scv_tr_handle::get_scv_tr_stream() const
{
  if ( (this->_scv_tr_handle_core_p == NULL) ||
	(this->_scv_tr_handle_core_p->my_scv_tr_generator_core_p == NULL) ){
    scv_tr_null_scv_tr_db_message();
    static scv_tr_stream* tmp_stream_p = new scv_tr_stream();
    return *tmp_stream_p;
  }

  return *(this->_scv_tr_handle_core_p->my_scv_tr_generator_core_p->
			my_scv_tr_stream_core_p->my_scv_tr_stream_p);
}

// ----------------------------------------------------------------------------

const scv_tr_generator_base&
		scv_tr_handle::get_scv_tr_generator_base() const
{
  if ( (this->_scv_tr_handle_core_p == NULL) ||
	(this->_scv_tr_handle_core_p->my_scv_tr_generator_core_p == NULL) ) {
    scv_tr_null_scv_tr_db_message();
    static scv_tr_generator_base* tmp_generator_base_p =
					new scv_tr_generator_base();
    return *tmp_generator_base_p;
  }

  return *(this->_scv_tr_handle_core_p->my_scv_tr_generator_core_p->
                        my_scv_tr_generator_base_p);
}

// ----------------------------------------------------------------------------

void scv_tr_handle::remove_callback(callback_h h)
{
  // There are 3 callback lists in scv_tr_handle

  {
    _scv_tr_callback_list* my_cb_list_p = _scv_tr_handle_core::callback_list_p;

    if (my_cb_list_p != NULL) {

      _scv_tr_callback_list::iterator i = my_cb_list_p->begin();

      for ( ; i != my_cb_list_p->end(); i++) {
        if ((*i)->id == h) {
          // The entry gets deleted the next time callbacks get processed -
          // you can't delete here because you might currently be in the middle of
          // a callback, and that would screw up the list if the item were deleted.
          (*i)->callback_fp = NULL;
          return;
        }
      }
    }
  }

  {
    _scv_tr_callback_list* my_cb_list_p =
      _scv_tr_handle_core::callback_relation_list_p;

    if (my_cb_list_p != NULL) {

      _scv_tr_callback_list::iterator i = my_cb_list_p->begin();

      for ( ; i != my_cb_list_p->end(); i++) {
        if ((*i)->id == h) {
          // The entry gets deleted the next time callbacks get processed -
          // you can't delete here because you might currently be in the middle of
          // a callback, and that would screw up the list if the item were deleted.
          (*i)->callback_fp = NULL;
          return;
        }
      }
    }
  }

  {
    _scv_tr_callback_list* my_cb_list_p =
      _scv_tr_handle_core::callback_record_attribute_list_p;

    if (my_cb_list_p != NULL) {

      _scv_tr_callback_list::iterator i = my_cb_list_p->begin();

      for ( ; i != my_cb_list_p->end(); i++) {
        if ((*i)->id == h) {
          // The entry gets deleted the next time callbacks get processed -
          // you can't delete here because you might currently be in the middle of
          // a callback, and that would screw up the list if the item were deleted.
          (*i)->callback_fp = NULL;
          return;
        }
      }
    }
  }
}

// ----------------------------------------------------------------------------

void scv_tr_handle::_end_transaction(
	const scv_extensions_if* exts_p,
	const sc_time& end_sc_time) const
{
  this->get_scv_tr_generator_base()._end_transaction(*this, exts_p, end_sc_time);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

scv_tr_handle scv_tr_generator_base::_begin_transaction(
                const scv_extensions_if* ext_p,
		const sc_time& begin_sc_time,
	        scv_tr_relation_handle_t relation_handle,
		const scv_tr_handle* other_handle_p) const
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_generator_base::_begin_transaction\n";
  cout << "  begin_sc_time = " << begin_sc_time.value() << endl;
#endif

  scv_tr_handle tp;

  if ( (this->_scv_tr_generator_core_p == NULL) ||
	(this->_scv_tr_generator_core_p->my_scv_tr_stream_core_p == NULL) ||
        (this->_scv_tr_generator_core_p->my_scv_tr_stream_core_p->
						my_scv_tr_db_core_p == NULL) ) {
#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_generator_base::_begin_transaction, NULL core\n";
#endif
    return tp;
  }

  tp._scv_tr_handle_core_p = new _scv_tr_handle_core;

  tp._scv_tr_handle_core_p->my_scv_tr_generator_core_p = 
		this->_scv_tr_generator_core_p;

  tp._scv_tr_handle_core_p->is_valid = true;
  tp._scv_tr_handle_core_p->is_active = true;
  tp._scv_tr_handle_core_p->begin_exts_p = ext_p;

  tp._scv_tr_handle_core_p->begin_sc_time = begin_sc_time;

  tp._scv_tr_handle_core_p->my_name =
		this->_scv_tr_generator_core_p->my_name;

  tp._scv_tr_handle_core_p->my_id = this->_scv_tr_generator_core_p->
			my_scv_tr_stream_core_p->my_scv_tr_db_core_p->
				global_transaction_id++;

  tp._scv_tr_handle_core_p->immediate_relation_handle = relation_handle;
  tp._scv_tr_handle_core_p->immediate_other_transaction_handle_p =
				other_handle_p;

  process_callbacks<
                scv_tr_handle,
                scv_tr_handle::callback_reason,
                scv_tr_handle::callback_function> (
		tp,
                _scv_tr_handle_core::callback_list_p,
                scv_tr_handle::BEGIN);

  if (relation_handle != 0) {
    tp.add_relation(relation_handle, *other_handle_p);
  }

  tp._scv_tr_handle_core_p->begin_exts_p = NULL;

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_generator_base::_begin_transaction\n";
#endif

  return tp;
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::_end_transaction(
		const scv_tr_handle& t,
                const scv_extensions_if* ext_p,
                const sc_time& end_sc_time) const
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_generator_base::_end_transaction\n";
#endif

  if (t._scv_tr_handle_core_p == NULL) return;

  if (t._scv_tr_handle_core_p->is_active == false) {
    scv_out <<
"Error in scv_tr_generator_base::end_transaction, transaction is not active.\n";
    return;
  }

  t._scv_tr_handle_core_p->end_exts_p = ext_p;
  t._scv_tr_handle_core_p->end_sc_time = end_sc_time;

  process_callbacks<
                scv_tr_handle,
                scv_tr_handle::callback_reason,
                scv_tr_handle::callback_function> (
                t,
                _scv_tr_handle_core::callback_list_p,
                scv_tr_handle::END);

  t._scv_tr_handle_core_p->is_active = false;

  t._scv_tr_handle_core_p->end_exts_p = NULL;

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_generator_base::_end_transaction\n";
#endif

  return;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

scv_tr_generator_base::scv_tr_generator_base(
        const char* name,
        scv_tr_stream& s,
        const char* begin_attribute_name,
        const char* end_attribute_name)
{
  this->_scv_tr_generator_core_p = new _scv_tr_generator_core(
		s._scv_tr_stream_core_p,
		this,
		name,
		begin_attribute_name,
		end_attribute_name);
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::_set_begin_exts_p(
		const scv_extensions_if* _begin_exts_p)
{
  this->_scv_tr_generator_core_p->begin_exts_p = _begin_exts_p;
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::_set_end_exts_p(
                const scv_extensions_if* _end_exts_p)
{
  if (this->_scv_tr_generator_core_p == NULL) return;

  this->_scv_tr_generator_core_p->end_exts_p = _end_exts_p;
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::_process_callbacks()
{
#ifdef scv_tr_TRACE
  cout << "Entering scv_tr_generator_base::_process_callbacks\n";
#endif

  process_callbacks<
                scv_tr_generator_base,
                scv_tr_generator_base::callback_reason,
                scv_tr_generator_base::callback_function> (
                *this,
                _scv_tr_generator_core::callback_list_p,
		scv_tr_generator_base::CREATE);

#ifdef scv_tr_TRACE
  cout << "Leaving scv_tr_generator_base::_process_callbacks\n";
#endif
}

// ----------------------------------------------------------------------------

scv_tr_generator_base::~scv_tr_generator_base()
{
#ifdef scv_tr_TRACE
  cout << "Entering ~scv_tr_generator_base\n";
#endif

  process_callbacks<
                scv_tr_generator_base,
                scv_tr_generator_base::callback_reason,
                scv_tr_generator_base::callback_function> (
                *this,
                _scv_tr_generator_core::callback_list_p,
                scv_tr_generator_base::DELETE);

#ifdef scv_tr_TRACE
  cout << "Entering ~scv_tr_generator_base\n";
#endif

  delete this->_scv_tr_generator_core_p;
}

// ----------------------------------------------------------------------------

scv_tr_db::callback_h scv_tr_generator_base::register_class_cb(
		callback_function* cbf,
		void* user_data_p)
{
  if (_scv_tr_generator_core::callback_list_p == NULL) {
    _scv_tr_generator_core::callback_list_p = new _scv_tr_callback_list;
  }

  _scv_tr_callback_item_t* my_item_p = new _scv_tr_callback_item_t;

  my_item_p->callback_fp = (void*) cbf;
  my_item_p->user_data_p = (void*) user_data_p;

  _scv_tr_generator_core::callback_list_p->push_back(my_item_p);

  return my_item_p->id;
}

// ----------------------------------------------------------------------------

scv_tr_relation_handle_t scv_tr_db::create_relation(
        const char* relation_name) const
{
  // Create a new relation that can be defined between two transactions.
  // If a relation with relation_name had previously been created, then
  // return the handle to that relation.

  scv_tr_relation_handle_t tmp_handle =
		this->_scv_tr_db_core_p->relation_by_name_map[relation_name];

  if (tmp_handle == 0) {
    tmp_handle = ++this->_scv_tr_db_core_p->relation_handle_counter;
    this->_scv_tr_db_core_p->relation_by_name_map[relation_name] = tmp_handle;
    this->_scv_tr_db_core_p->relation_by_handle_map[tmp_handle] = relation_name;
  }

  return tmp_handle;
}

// ----------------------------------------------------------------------------

const char* scv_tr_db::get_relation_name(
        scv_tr_relation_handle_t relation_handle) const
{
  return this->_scv_tr_db_core_p->
			relation_by_handle_map[relation_handle].c_str();
}

// ----------------------------------------------------------------------------

uint64 scv_tr_generator_base::get_id() const
{
  if (this->_scv_tr_generator_core_p == NULL) return 0;
  return this->_scv_tr_generator_core_p->my_id;
}

// ----------------------------------------------------------------------------

const scv_extensions_if* scv_tr_generator_base::get_begin_exts_p() const
{
  if (this->_scv_tr_generator_core_p == NULL) return NULL;

  const scv_extensions_if* my_exts_p =
                                this->_scv_tr_generator_core_p->begin_exts_p;

  // Check if default template parameter is used, if so return NULL:
  //
  if (my_exts_p->get_type() == scv_extensions_if::RECORD) {
    if ( (my_exts_p->get_field(0)->get_type() == scv_extensions_if::INTEGER) &&
         ( ! strcmp(my_exts_p->get_field(0)->get_name(),"_default_field_") ) ) {
      return NULL;
    }
  }

  return my_exts_p;
}

// ----------------------------------------------------------------------------

const scv_extensions_if* scv_tr_generator_base::get_end_exts_p() const
{
  if (this->_scv_tr_generator_core_p == NULL) return NULL;

  const scv_extensions_if* my_exts_p =
                                this->_scv_tr_generator_core_p->end_exts_p;

  // Check if default template parameter is used, if so return NULL:
  //
  if (my_exts_p->get_type() == scv_extensions_if::RECORD) {
    if ( (my_exts_p->get_field(0)->get_type() == scv_extensions_if::INTEGER) &&
         ( ! strcmp(my_exts_p->get_field(0)->get_name(),"_default_field_") ) ) {
      return NULL;
    }
  }

  return my_exts_p;
}

// ----------------------------------------------------------------------------

const scv_tr_stream& scv_tr_generator_base::get_scv_tr_stream() const
{
  if ( (this->_scv_tr_generator_core_p == NULL) ||
	(this->_scv_tr_generator_core_p->my_scv_tr_stream_core_p == NULL) ) {

    scv_tr_null_scv_tr_db_message();

    // This will be an invalid scv_tr_stream:
    //
    static scv_tr_stream* tmp_stream_p = new scv_tr_stream();

    return *tmp_stream_p;
  }

  return *this->_scv_tr_generator_core_p->my_scv_tr_stream_core_p->
			my_scv_tr_stream_p;
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::print(ostream& o, int details, int indent) const
{
  o << "scv_tr_generator_base: " << this->get_name() << endl;
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::show(int details, int indent) const
{
  this->print(scv_out, details, indent);
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::set_debug(int d)
{
  _scv_tr_generator_core::debug = d;
}

// ----------------------------------------------------------------------------

int scv_tr_generator_base::get_debug()
{
  return _scv_tr_generator_core::debug;
}

// ----------------------------------------------------------------------------

void scv_tr_generator_base::remove_callback(callback_h h)
{
  _scv_tr_callback_list* my_cb_list_p = _scv_tr_generator_core::callback_list_p;

  if (my_cb_list_p == NULL) return;

  _scv_tr_callback_list::iterator i = my_cb_list_p->begin();

  for ( ; i != my_cb_list_p->end(); i++) {
    if ((*i)->id == h) {
      // The entry gets deleted the next time callbacks get processed -
      // you can't delete here because you might currently be in the middle of
      // a callback, and that would screw up the list if the item were deleted.
      (*i)->callback_fp = NULL;
    }
  }
}

// ----------------------------------------------------------------------------

const char *scv_tr_generator_base::get_name() const
{
  if (this->_scv_tr_generator_core_p == NULL) {
    static std::string tmp_name = "<anonymous>";
    return tmp_name.c_str();
  }

  static std::string tmp_my_name;
  tmp_my_name = this->_scv_tr_generator_core_p->my_name;
  return tmp_my_name.c_str();
}

// ----------------------------------------------------------------------------

const char* scv_tr_generator_base::get_begin_attribute_name() const
{
  return this->_scv_tr_generator_core_p->my_begin_attribute_name.c_str();
}

// ----------------------------------------------------------------------------

const char* scv_tr_generator_base::get_end_attribute_name() const
{
  return this->_scv_tr_generator_core_p->my_end_attribute_name.c_str();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

scv_tr_handle scv_tr_stream::get_current_transaction_handle()
{
  // TBD, need a stack of scv_tr_handle on each stream
  return scv_tr_handle();
}

// ----------------------------------------------------------------------------

const scv_tr_db* scv_tr_stream::get_scv_tr_db() const
{
  return this->_scv_tr_stream_core_p->my_scv_tr_db_core_p->my_scv_tr_db_p;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
