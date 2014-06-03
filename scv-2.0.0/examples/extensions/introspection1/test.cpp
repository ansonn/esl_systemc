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
// 
// This prototype was created by C. Norris Ip, Stuart Swan, and Jasvinder Singh
// from Cadence Design Systems, Inc., and distributed to the SystemC
// Verification Working Group as use case scenarios and potential adaptation. 

/*
 * Author:  C. Norris Ip and Stuart Swan
 * Date :   2/4/2002
 *
 * This is a basic example on introspection.
 * (VWG Rb1 : arbitrarily complex data type must be supported)
 *
 * SystemC Verification Working Group requirement are noted by "VWG".
 * (VWG R31, R35, R3d, Rb1, Rb2, Rb3)
 * 
 */

// ----------------------------------------
// this header file "scv_introspection.h" 
// provides the interface from which
// arbitrary data type can be manipulated, including
//  + int
//  + unsigned
//  + custom composite type
//  + etc.
// 
// When "systemc.h" is included before this header file,
// it also enable manipulation of SystemC types such as:
//  + sc_uint
//  + sc_bv
//
//  The interface includes things like:
//
//    class scv_extensions_if {
//    public:
//      virtual int get_num_fields() const=0;        // type information
//      virtual scv_extensions_if * get_field(int)=0; // type information
//      ...
//      virtual long long get_integer() const=0;     // value information
//      ...
//      virtual void next()=0;                 // randomization
//      virtual void uninitialize()=0;         // randomization
//      virtual void initialize()=0;           // randomization
//      ...
//    };
//
//  The user may use a template function and a template class
//  from this header file to access the scv_extensions_if interface:
//
//    template<class T>
//    scv_extensions<T> scv_get_extensions(T& data) { ... }  
//
//    template<class T>
//    const scv_extensions<T> scv_get_const_extensions(const T& data) { ... }  
//
//    template<class T>
//    class scv_smart_ptr : ... { ... }
//
//  The reminder of the example illustrates how these can be used
//  in custom composite types to perform constrained randomization.
//
// ----------------------------------------
#include "scv.h"

#define RND_SEED 12345

// ----------------------------------------
// a custom composite type definition
// in user's code or legacy code
//
// for illustration purpose, let's assume
// it is in a read-only header file and
// we cannot modify this definition (VWG R3d, Rb1)
// ----------------------------------------

struct packet_t {
  int packet_type;
  int src;
  int dest;
  int payload;
};

// ----------------------------------------
// several procedures in user's code or legacy code
// that utilizes the "packet_t" type.
//
// for illustration purpose, let's assume
// it is in a read-only header file and
// we cannot modify this definition. (VWG Rb1)
// ----------------------------------------

ostream& operator<<(ostream& os, const packet_t& p) {
  os << "packet : " << endl;
  os << p.packet_type << endl;
  os << p.src << endl;
  os << p.dest << endl;
  os << p.payload << endl;
  os << "end_packet" << endl;
  return os;
}

void my_legacy_procedure(const packet_t& data) {
  // ...
  cout << data;
}

// ----------------------------------------
// during the import of composite type, 
// an extension is either manually written or
// automatically generated.
//
// note that using this extension does not require
// modification of the "struct" definition.
//
// it may be tedious to do this manually, but
// it is quite mechanical and can be easily automated.
// ----------------------------------------

//
// the user of packet_t does not even need to know
// that this class exists.
//
// the importer of a composite type is probably a transactor creator.
//
template<>
class scv_extensions<packet_t> : public scv_extensions_base<packet_t> {
public:
  scv_extensions<int> packet_type;
  scv_extensions<int> src;
  scv_extensions<int> dest;
  scv_extensions<int> payload;

  SCV_EXTENSIONS_CTOR(packet_t) {
	  SCV_FIELD(packet_type);
      SCV_FIELD(src);
      SCV_FIELD(dest);
      SCV_FIELD(payload);
  }
};

// ----------------------------------------
// once an extension has been declared for 
// a custom composite type, information
// can be extracted from it using a PLI-like 
// interface in a *generic* library routine.
//
// These PLI-like interfaces can be accessed from
//   * a routine written by an internal CAD group
//   * a routine in the SystemC library
//   * a routine in a third-vendor library
//
// This is for tool vendors only: users writing their
// testbenches do not necessarily
// need to know how to write this kind of procedures.
//
// While this example shows an interface with only 
// basic methods such as "get_num_fields()", new 
// methods can be added easily to the facility
// without requiring the user to rewrite their existing testbenches.
// The implementation is modularized so a new 
// extension component can be added without (or minimal)
// changes to the other components.
// 
// ----------------------------------------

// e.g. type information (VWG Rb2)
template<class T>
void print_number_of_fields(const T& data) {
  cout << "number of fields : " 
       << scv_get_const_extensions(data).get_num_fields() << endl;
}

// e.g. value access (VWG Rb3)
template<class T>
void print_fields(const T& data) {

  // need a copy of the extension that persists until the end
  const scv_extensions<T> ext = scv_get_const_extensions(data);

  // print values in the fields
  int size = ext.get_num_fields();
  if (size) {
    cout << "Printing field values..." << endl;
    for (int i=0; i<size; ++i) {
      const scv_extensions_if * field = ext.get_field(i);
      if (field->is_integer())
	cout << "field \"" << field->get_name()
	     << "\" has value " << field->get_integer() << "." << endl;
    }
  }
}

SC_MODULE(sctop) {
  SC_CTOR(sctop);
};

#ifdef NCSC
  SC_MODULE_EXPORT(sctop);
#endif

// ----------------------------------------
// example usage
//
// what the user writing a testbench would manipulate
// their composite types.
// ----------------------------------------
sctop::sctop(sc_module_name name) : sc_module(name)
{

  // tb_startup();

  // ----------------------------------------
  // static extension (does not require extra storage space in a packet)
  // ----------------------------------------
  packet_t p;
  // Define the characteristics for this structure, then make sure introspection agrees
  int numFields = 4;
  std::string fieldNames[] = {
	  "packet_type",
	  "src",
	  "dest",
	  "payload"
  };
  int fieldValues[] = {0, 0, 1, 2};
  int errorCount = 0;

  // Initialize the fields
  p.packet_type = fieldValues[0];
  p.src = fieldValues[1];
  p.dest = fieldValues[2];
  p.payload = fieldValues[3];

  // Check introspection functions
  const scv_extensions<packet_t> ext = scv_get_const_extensions(p);
  int size = ext.get_num_fields();
  if (size != numFields) {
	  errorCount++;
	  cout << TBS_ERROR << "Number of fields is " << size << ", should be " << numFields << endl;
  } else {
	  cout << TBS_INFO << "Number of fields is " << size << endl;
  }
  if (size > 0 && size <= numFields) {
	  for (int i=0; i<size; ++i) {
		  const scv_extensions_if * field = ext.get_field(i);
		  if (field == NULL) {
			  errorCount++;
			  cout << TBS_ERROR << "Field " << i << " not found" << endl;
		  } else if (fieldNames[i] != field->get_name()) {
			  errorCount++;
			  cout << TBS_ERROR << "Field " << i << " should have name " << fieldNames[i] << ", instead has name " << field->get_name() << endl;
		  } else if (!field->is_integer()) {
			  errorCount++;
			  cout << TBS_ERROR << "Expected " << fieldNames[i] << " to be of type integer" << endl;
		  } else {
			  if (field->get_integer() != fieldValues[i]) {
				  errorCount++;
				  cout << TBS_ERROR << "Expected value " << fieldValues[i] << " for field " << fieldNames[i] << " found to instead be " << field->get_integer() << endl;
			  } else {
				  cout << TBS_INFO << "Field " << i << " okay" << endl;
			  }			  
		  }
	  }
  }

  // ----------------------------------------
  // dynamic extension (allow attachment of callbacks,
  // uninitialized values, constraints, etc.)
  //
  // - require the use of scv_smart_ptr template.
  // ----------------------------------------

  // randomization (direct) (VWG R3d, Rb1)
  scv_smart_ptr<packet_t> pp;
  scv_shared_ptr<scv_random> g(new scv_random("gen", RND_SEED));
  pp->set_random(g);

  //
  // Check introspection functions
  //

  fieldValues[0] = 17;
  fieldValues[1] = 34;
  fieldValues[2] = 63;
  fieldValues[3] = -13;
  pp->packet_type = fieldValues[0];
  pp->src = fieldValues[1];
  pp->dest = fieldValues[2];
  pp->payload = fieldValues[3];
  scv_extensions_if *ppExt = pp.get_extensions_ptr();
  size = ppExt->get_num_fields();
  if (size != numFields) {
	  errorCount++;
	  cout << TBS_ERROR << "Number of fields is " << size << ", should be " << numFields << endl;
  } else {
	  cout << TBS_INFO << "Number of fields is " << size << endl;
  }
  if (size > 0 && size <= numFields) {
	  for (int i=0; i<size; ++i) {
		  const scv_extensions_if * field = ppExt->get_field(i);
		  if (field == NULL) {
			  errorCount++;
			  cout << TBS_ERROR << "Field " << i << " not found" << endl;
		  } else if (fieldNames[i] != field->get_name()) {
			  errorCount++;
			  cout << TBS_ERROR << "Field " << i << " should have name " << fieldNames[i] << ", instead has name " << field->get_name() << endl;
		  } else if (!field->is_integer()) {
			  errorCount++;
			  cout << TBS_ERROR << "Expected " << fieldNames[i] << " to be of type integer" << endl;
		  } else {
			  if (field->get_integer() != fieldValues[i]) {
				  errorCount++;
				  cout << TBS_ERROR << "Expected value " << fieldValues[i] << " for field " << fieldNames[i] << " found to instead be " << field->get_integer() << endl;
			  } else {
				  cout << TBS_INFO << "Field " << i << " okay" << endl;
			  }			  
		  }
	  }
  }

  //
  // Check uninitialize, initialize and next
  //

  cout << TBS_INFO << "Check if maintains initial value across consecutive accesses" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  if (fieldValues[0] != pp->packet_type || fieldValues[1] != pp->src ||
	  fieldValues[2] != pp->dest || fieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed across consecutive accesses" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained across consecutive accesses" << endl;
  }

  cout << TBS_INFO << "Make sure they change values after a next" << endl;
  pp->next();
  int newFieldValues[4];
  newFieldValues[0] = pp->packet_type;
  newFieldValues[1] = pp->src;
  newFieldValues[2] = pp->dest;
  newFieldValues[3] = pp->payload;
  if (fieldValues[0] == newFieldValues[0]
      || fieldValues[1] == newFieldValues[1]
      || fieldValues[2] == newFieldValues[2]
      || fieldValues[3] == newFieldValues[3])
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) did not change after next" << endl;
  } else {
	  cout << TBS_INFO << "Values changed after next" << endl;
  }

  cout << TBS_INFO << "Check if maintains next value across multiple consecutive accesses" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed across consecutive accesses" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained across consecutive accesses" << endl;
  }

  cout << TBS_INFO << "Check if maintains value across spurious initialize" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  pp->initialize();
  if (fieldValues[0] != pp->packet_type || fieldValues[1] != pp->src ||
	  fieldValues[2] != pp->dest || fieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed across spurious initialize" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained across spurious initialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after uninitialize" << endl;
  pp->uninitialize();
  if (fieldValues[0] == (newFieldValues[0] = pp->packet_type) || fieldValues[1] == (newFieldValues[1] = pp->src) ||
	  fieldValues[2] == (newFieldValues[2] = pp->dest) || fieldValues[3] == (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) didn't change across uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values changed across uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after 2nd access after uninitialize" << endl;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed after 2nd access after uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained after 2nd access after uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after spurious uninitialize" << endl;
  pp->uninitialize();
  pp->uninitialize();
  if (fieldValues[0] == (newFieldValues[0] = pp->packet_type) || fieldValues[1] == (newFieldValues[1] = pp->src) ||
	  fieldValues[2] == (newFieldValues[2] = pp->dest) || fieldValues[3] == (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) didn't change across uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values changed across uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after 2nd access after spurious uninitialize" << endl;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed after 2nd access after uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained after 2nd access after uninitialize" << endl;
  }

  //
  // Repeat the above checks, but this time only change 1st and 3rd fields
  //

  cout << TBS_INFO << "Repeat above checks, but only changing 1st and 3rd fields" << endl;

  cout << TBS_INFO << "Make sure they change values after a next" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  pp->packet_type.next();
  pp->dest.next();
  if (fieldValues[0] == (newFieldValues[0] = pp->packet_type) || fieldValues[1] != (newFieldValues[1] = pp->src) ||
	  fieldValues[2] == (newFieldValues[2] = pp->dest) || fieldValues[3] != (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value did not change properly after next" << endl;
  } else {
	  cout << TBS_INFO << "Values changed properly after next" << endl;
  }

  cout << TBS_INFO << "Check if maintains next value across multiple consecutive accesses" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed across consecutive accesses" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained across consecutive accesses" << endl;
  }

  cout << TBS_INFO << "Check if maintains value across spurious initialize" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  pp->packet_type.initialize();
  pp->dest.initialize();
  if (fieldValues[0] != pp->packet_type || fieldValues[1] != pp->src ||
	  fieldValues[2] != pp->dest || fieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed across spurious initialize" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained across spurious initialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after uninitialize" << endl;
  pp->packet_type.uninitialize();
  pp->dest.uninitialize();
  if (fieldValues[0] == (newFieldValues[0] = pp->packet_type) || fieldValues[1] != (newFieldValues[1] = pp->src) ||
	  fieldValues[2] == (newFieldValues[2] = pp->dest) || fieldValues[3] != (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) didn't change properly across uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values changed properly across uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after 2nd access after uninitialize" << endl;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed after 2nd access after uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained after 2nd access after uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after spurious uninitialize" << endl;
  pp->packet_type.uninitialize();
  pp->dest.uninitialize();
  pp->packet_type.uninitialize();
  pp->dest.uninitialize();
  if (fieldValues[0] == (newFieldValues[0] = pp->packet_type) || fieldValues[1] != (newFieldValues[1] = pp->src) ||
	  fieldValues[2] == (newFieldValues[2] = pp->dest) || fieldValues[3] != (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) didn't change properly across uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values changed properly across uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after 2nd access after spurious uninitialize" << endl;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed improperly after 2nd access after uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values properly maintained after 2nd access after uninitialize" << endl;
  }

  //
  // Repeat the above checks, but this time only change 2nd and 4th fields
  //

  cout << TBS_INFO << "Repeat above checks, but only changing 1st and 3rd fields" << endl;

  cout << TBS_INFO << "Make sure they change values after a next" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  pp->src.next();
  pp->payload.next();
  if (fieldValues[0] != (newFieldValues[0] = pp->packet_type) || fieldValues[1] == (newFieldValues[1] = pp->src) ||
	  fieldValues[2] != (newFieldValues[2] = pp->dest) || fieldValues[3] == (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value did not change properly after next" << endl;
  } else {
	  cout << TBS_INFO << "Values changed properly after next" << endl;
  }

  cout << TBS_INFO << "Check if maintains next value across multiple consecutive accesses" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed across consecutive accesses" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained across consecutive accesses" << endl;
  }

  cout << TBS_INFO << "Check if maintains value across spurious initialize" << endl;
  fieldValues[0] = pp->packet_type;
  fieldValues[1] = pp->src;
  fieldValues[2] = pp->dest;
  fieldValues[3] = pp->payload;
  pp->src.initialize();
  pp->payload.initialize();
  if (fieldValues[0] != pp->packet_type || fieldValues[1] != pp->src ||
	  fieldValues[2] != pp->dest || fieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed across spurious initialize" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained across spurious initialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after uninitialize" << endl;
  pp->src.uninitialize();
  pp->payload.uninitialize();
  if (fieldValues[0] != (newFieldValues[0] = pp->packet_type) || fieldValues[1] == (newFieldValues[1] = pp->src) ||
	  fieldValues[2] != (newFieldValues[2] = pp->dest) || fieldValues[3] == (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) didn't change properly across uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values changed properly across uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after 2nd access after uninitialize" << endl;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed after 2nd access after uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values maintained after 2nd access after uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after spurious uninitialize" << endl;
  pp->src.uninitialize();
  pp->payload.uninitialize();
  pp->src.uninitialize();
  pp->payload.uninitialize();
  if (fieldValues[0] != (newFieldValues[0] = pp->packet_type) || fieldValues[1] == (newFieldValues[1] = pp->src) ||
	  fieldValues[2] != (newFieldValues[2] = pp->dest) || fieldValues[3] == (newFieldValues[3] = pp->payload))
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) didn't change properly across uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values changed properly across uninitialize" << endl;
  }

  cout << TBS_INFO << "Check behavior after 2nd access after spurious uninitialize" << endl;
  if (newFieldValues[0] != pp->packet_type || newFieldValues[1] != pp->src ||
	  newFieldValues[2] != pp->dest || newFieldValues[3] != pp->payload )
  {
	  errorCount++;
	  cout << TBS_ERROR << "Value(s) changed improperly after 2nd access after uninitialize" << endl;
  } else {
	  cout << TBS_INFO << "Values properly maintained after 2nd access after uninitialize" << endl;
  }

  if (errorCount != 0) {
	  cout << TBS_FAIL << "Encountered " << errorCount << " errors\n";
  } else {
	  cout << TBS_SUCCESS << endl;
  }
}

int sc_main(int argc, char** argv) {
  sctop top("top");  //for osci
  sc_start();
  return 0;
}
