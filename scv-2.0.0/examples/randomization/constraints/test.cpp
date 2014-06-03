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
 test.cpp -- 
  
 - Illustrates SCV introspection and constrained randomization concepts.
 - Shows distinction between transaction level data and constraints 
 - Shows ways of attaching different constraints to data objects in tests
 - Shows ways of generating new random values and changing constraints in tests
   for a reusable methodology.
 - Also illustrates some methodologies which should be followed for writing 
   reusable and efficient testbenches for manipulating data and managing 
   constraints
 *****************************************************************************/

#include "packet_ext.h"
#include "packet_constraints.h"
#include "constraints.h"

const int num_iter = 10;

///////////////////////////////////////////////////////////////////////
// Function: testbench_generate_packets
// Description: 
//   - Shows concept of data and constraints in SCV
//   - Demonstrates randomization of transaction data 
//   - Demonstrates various mechanisms provided in SCV for constraining
//     data object
//     * Constraints using keep_only/keep_out and distributions
//     * Constraints using constraint classes derived from scv_constraint_base
//     * Attaching constraints to data objects (use_constraint)
//     * Obtaining values for interdependent variables
///////////////////////////////////////////////////////////////////////

void testbench_generate_packets() {

   /* ******* Unconstrained packet randomization  ******** */

   // Note that transaction level data (composite data types) extended
   // using introspection in SCV do not have any default constraints
   // attached with those objects. Every instance of that object
   // can have seperate constraints attached either using distributions
   // and ranges of values or can have complex constraint expressions
   // specified using constraint classes, classes obtained inheriting
   // from scv_constraint_base.

   {
     cout << "Generate unconstrained packet data" << endl;
     scv_smart_ptr<packet> smart_p_ptr;
  
     for (int i =0; i < num_iter; i++) {
       smart_p_ptr->next();
       smart_p_ptr->read().dump();
     }
   }

   /* *** Constrained randomization using range/distribution constraints *** */
  
   // Previous example showed how to generate random transaction leve
   // data with constraints specified essentially by the type of the 
   // data objects.
   // This example will shows creating and attaching disjoint ranges of
   // legal values and distributions to instances of data objects.

   {
     cout << "Generate constrained (range/distribution) packet data" << endl;
     scv_smart_ptr<packet> smart_p_ptr;
     scv_bag<sc_uint<8> > dist;

     dist.add(10, 50); // specify 10 as legal value and 50% weight 
     dist.add(15, 30); // specify 15 as legal value and 30% weight
     dist.add(20, 20); // specify 20 as legal value and 20% weight

     smart_p_ptr->ptype.keep_only(UNICAST); // Note that these constraints 
     smart_p_ptr->source.keep_only(0, 0xf); // become part of the object
     smart_p_ptr->destination.set_mode(dist); // set distribution of values 

     for (int i =0; i < num_iter; i++) {
       smart_p_ptr->next();
       smart_p_ptr->read().dump();
     }
    
   }

   /* **** Constrained randomization using constraint class **** */

   // Above examples have shown how to generate unconstrained transaction
   // level data and also attaching and disjoint ranges of value and 
   // distributions with instances of transaction level data object.
   // This example will focus on how to create any arbitrary constraint
   // using expressions and even the above mentioned means using
   // constraint classes, encapsulating the transaction level data object
   // The encapsulated data object is points to the container constraint
   // class object. In that sense it is different from any other scv_smart_ptr
   // object of that particular data type.

   // So the constraint class defines a way of defining constraints not
   // just on single but multiple dependent objects which can be of any 
   // type, composite or scalar types (C++ or SystemC) types.a

   // One can use virtual next() method on the scv_constraint_base for
   // generating new values for the constraint object. SCV also allows 
   // obtaining new values for individual embedded elements using next()
   // on specific elements.

   // This specific example shows how to create a constriant object and
   // generate a new random value for the contained transaction level object
   // it also shows how the value is copied from the embedded object to the
   // object instantiated in the test (smart_p_ptr).

   {
     cout << "Generate constrained packets using constraint class" << endl;

     qualis_packet pc("pc");
     scv_smart_ptr<packet> smart_p_ptr;
 
     cout << " Constraint class as generator" << endl; 
     for (int i=0; i < num_iter; i++) {
       pc.next();  // obtain new constrained packet value for packet
                   // object contained in the constraint class
       smart_p_ptr.write(pc.sp.read()); // smart_p_ptr contains the new random 
                                        // value. smart_p_ptr in itself does 
                                        // not have any constraints attached
                                        // with it

       smart_p_ptr->read().dump();
     }
     cout << " Next on original object (unconstrained)" << endl; 
     for (int i=0; i < num_iter; i++) {
       smart_p_ptr->next(); // Will result in unconstrained random values
       smart_p_ptr->read().dump();
     }
   }

   /* ***  Constrained randomization for packet using use_constraint *** */

   // In certain situations it is desired that constraints be associated 
   // with a data object in the test and not just for objects instantiated
   // in the constraint class.

   // SCV allows this by use_constraint method. use_constraint results in 
   // copying constraints associated with the argument object to the object
   // on which use_constraint is specified. Copying makes this scv_smart_ptr
   // object completely independent of the object and even when the other
   // object or the associated constraint object goes out of scope this is
   // not affected. Also if the dependent fields change elsewhere those 
   // do not effect this particular instance. More on dependent items
   // explained with examples below 

   // NOTE that use_constraint cannot be applied to scv_smart_ptr objects
   // embedded in the constraint class. 

   // There are few subtle issues related to interdependent constraint
   // variables which a user should be aware off.

   // Important thing to keep in mind with use_constraints
   // Can be applied to all kinds of constraints eg. distributions and
   // fields within constraint classes. 

   // Attaches a constraint instance and particular field within
   // constraint instance to that object. It just does not start 
   // pointing or refering to that field in that constraint instance.
   // So the constraint object associated with it can go out of scope
   // without effecting the object on which use_constraint was applied

   // Another thing to notice is that constraint object can contain
   // multiple fields which can be scalar data types or composite types
   // If you apply use_constraint using a field of a constraint class
   // which depends on another field, then the value of the dependent
   // field at the instance use_constraint was called will be used
   // to generate new set of random values for this field. 

   // This subsection will show the following
   //   - use_constraint on an instance of composite type "packet", where  
   //     constraints are interdependent on the fields of composite type.
   //   - use_constraint from a field of constraint class dependent on 
   //     another field
   //   - next() on instance of constraint class and specific elements
   //     in the constraint class instance

   {

     cout << "Generate constrained packets using use_constraint" << endl;  
     {
       qualis_packet pc("pc");
       scv_smart_ptr<packet> smart_p_ptr;
  
       smart_p_ptr->use_constraint(pc.sp); // Associate constraints
                                             // with an object declared in the
                                             // test
       for (int i=0; i < num_iter; i++) {
         smart_p_ptr->next(); // note next() on the smart_p_ptr itself and 
                              // no copying of data happening here
         smart_p_ptr->read().dump();
       }
     }


     cout << "Generate constrained integer values using use_constraint" << endl;  
     {
       dependent_integer_constraints dic("dic");
       scv_smart_ptr<int> ivar;
   
       cout << " One value for constrained object" << endl;  
       print_constraint(dic); // Print the current values for constraint object
       ivar->use_constraint(dic.ivar); 
      
       cout << " Values for ivar object using use_constraint" << endl;  
       for (int i=0; i < num_iter; i++) {
         ivar->next();
         cout << "  ivar: " << *ivar << endl;
       } 
     }

     cout << "Constraint and dependent variable next(). " << endl;  
     {
       dependent_integer_constraints dic_1("dic_1");
      
       cout << " Next on constraint object" << endl; 
       for (int i=0; i < num_iter; i++) {
         dic_1.next();
         print_constraint(dic_1);
       } 
  
       cout << " Next on ivar field on constraint object" << endl; 
       for (int i=0; i < num_iter; i++) {
         dic_1.ivar->next();
         print_constraint(dic_1);
       }
   
       cout << " Next on jvar field on constraint object" << endl; 
       for (int i=0; i < num_iter; i++) {
         dic_1.jvar->next();
         print_constraint(dic_1);
       } 
     }
   }
  
   /* **  Change simple constraints in a test and use_constraint ** */ 

   // Changing constraints on fields of composite type and attaching 
   // constraints using use_constraints
   // Explicitly shows creating multiple instance of packet_constraints
   // in the test and defining constraints once and reapplying those
   // multiple times in the test. Setting constraints once and reusing
   // is more efficient than doing processing every time to do say a 
   // combination of 3 sets (while generating millions of transaction 
   // data values).

   cout << "Change range/distribution constraints dynamically and use_constraint" << endl;
   {
     qualis_packet pc("pc");
     scv_smart_ptr<packet> smart_p_ptr_1;
     scv_smart_ptr<packet> smart_p_ptr_2;
   
     pc.sp->ptype.reset_distribution();
     pc.sp->ptype.keep_only(UNICAST, MULTICAST);
     smart_p_ptr_1->use_constraint(pc.sp);

     cout << " First set of constraints (UNICAST, MULTICAST)" << endl;
     for (int i=0; i < num_iter; i++) {
       smart_p_ptr_1->next();
       smart_p_ptr_1->read().dump();
     }

     cout << " Second set of constraints (MULTICAST)" << endl;
     pc.sp->ptype.reset_distribution();
     pc.sp->ptype.keep_only(MULTICAST);
     smart_p_ptr_2->use_constraint(pc.sp);

     for (int i=0; i < num_iter; i++) {
       smart_p_ptr_2->next();
       smart_p_ptr_2->read().dump();
     }
   }
  
   /* ** Attach different sets of constraints to an object ** */

   // Keep the constraint classes small, helps in debugging and 
   // useful for extensions etc. for hierarchical constraint description

   // So the Verification engineer can define various different
   // constraint classes and either have those visible to the test
   // writer or just have some global method defined for the
   // test writer to choose either one based on some criterion.

   // This example will show creation of 2 differnt constraint 
   // classes rather than one "packet_constraints" containing 
   // multiple instances of transaction level data types eg. packet.

   // A method 'constrain_packet' is also provided to let the test
   // writers use attach specific constraints based on the test description.
   // packet_constraints, default_packet, intermediate_packet are
   // the constraint classes defined. Having these seperate is good
   // for reuse methodology and efficient and easy to debug.

   cout << "Use static method constrain_packet to pick 3 predefined constraints" << endl;
   {
     scv_smart_ptr<packet> sp_1;
     scv_smart_ptr<packet> sp_2;
     scv_smart_ptr<packet> sp_3;
    
     constrain_packet(sp_1, QUALIS_PACKET);
     constrain_packet(sp_2, DEFAULT_PACKET);
     constrain_packet(sp_3, INTERMEDIATE_PACKET);

     cout << " Choose qualis_constraints" << endl;
     for (int i=0; i < num_iter; i++) {
       sp_1->next();
       sp_1->read().dump();
     }
     cout << " Choose default_constraints" << endl;
     for (int i=0; i < num_iter; i++) {
       sp_2->next();
       sp_2->read().dump();
     }
     cout << " Choose intermediate_constraints" << endl;
     for (int i=0; i < num_iter; i++) {
       sp_3->next();
       sp_3->read().dump();
     }
   }

   cout << "Exiting example" << endl;
}

SC_MODULE(sctop) {
  SC_CTOR(sctop);
};

#ifdef NCSC
  SC_MODULE_EXPORT(sctop);
#endif

sctop::sctop(sc_module_name name) : sc_module(name)
{
  scv_random::set_global_seed(scv_random::pick_random_seed());
  testbench_generate_packets();
}
  
int sc_main(int argc, char ** argv)
{
  sctop top("top");  //for osci
  sc_start();
  return 0;
}
 
