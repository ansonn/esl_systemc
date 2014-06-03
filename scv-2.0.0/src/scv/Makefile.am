## ****************************************************************************
##
##  The following code is derived, directly or indirectly, from the SystemC
##  source code Copyright (c) 1996-2014 by all Contributors.
##  All Rights reserved.
##
##  The contents of this file are subject to the restrictions and limitations
##  set forth in the SystemC Open Source License (the "License");
##  You may not use this file except in compliance with such restrictions and
##  limitations. You may obtain instructions on how to receive a copy of the
##  License at http://www.accellera.org/. Software distributed by Contributors
##  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
##  ANY KIND, either express or implied. See the License for the specific
##  language governing rights and limitations under the License.
##
## ****************************************************************************
##
##  Makefile.am --
##  Process this file with automake to produce a Makefile.in file.
##
## ****************************************************************************
##
##  Original Authors:
##  Norris Ip, Dean Shea, John Rose, Jasvinder Singh, William Paulsen,
##  John Pierce, Rachida Kebichi, Ted Elkind, David Bailey,
##  Cadence Design Systems, Inc., 2002-09-23
##
## ****************************************************************************
##
##  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
##  changes you are making here.
##
##      Name, Affiliation, Date: Eric Paire, Jerome Cornet, 
##                               STMicroelectronics, 2012-04-27
##  Description of Modification: Revamp of autotool build system for 64-bit
##                               support, 32-bit compilation on 64-bit host
##
##      Name, Affiliation, Date: Torsten Maehne,
##                               Universite Pierre et Marie Curie, 2013-05-04
##  Description of Modification: Merge in content of Makefile.extra and drop
##                               the file, as it is not referenced elsewhere.
##                               Move -L options to LDFLAGS and enclose
##                               Makefile vaiables with round brackets instead
##                               of curly ones.
##
## ****************************************************************************

lib_LTLIBRARIES = libscv.la

libscv_la_SOURCES = \
	scv_constraint.cpp \
	scv_constraint_range.cpp \
	scv_debug.cpp \
	scv_expression.cpp \
	scv_init_seed.cpp \
	scv_introspection.cpp \
	scv_random.cpp \
	scv_report.cpp \
	scv_tr.cpp \
	scv_tr_text.cpp \
	scv_util.cpp

my_includes_HEADERS = \
	_scv_associative_array.h \
	_scv_data_structure.h \
	_scv_ext_callbacks.h \
	_scv_ext_comp.h \
	_scv_ext_rand.h \
	_scv_ext_rw.h \
	_scv_ext_type.h \
	_scv_ext_util.h \
	_scv_introspection.h \
	_scv_list_iter.h \
	_scv_randomization.h \
	_scv_smart_ptr.h \
	scv_bag.h \
	scv_constraint.h \
	scv_constraint_range.h \
	scv_debug.h \
	scv_expression.h \
	scv_introspection.h \
	scv_messages.h \
	scv_object_if.h \
	scv_random.h \
	scv_report.h \
	scv_shared_ptr.h \
	scv_sparse_array.h \
	scv_tr.h \
	scv_util.h \
	scv_kit_date.h \
	scv_ver.h

my_includesdir = $(includedir)/scv

libscv_la_CPPFLAGS = \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/cudd/2.3.0/cudd \
	-I$(top_srcdir)/src/cudd/2.3.0/obj \
	-I$(top_srcdir)/src/cudd/2.3.0/util \
	-I$(top_srcdir)/src/cudd/2.3.0/mtr \
	-I$(top_srcdir)/src/cudd/2.3.0/st \
	-I$(SYSC_INC_PREFIX)

libscv_la_LIBADD = \
	$(top_builddir)/src/cudd/2.3.0/obj/libobj.la \
	$(top_builddir)/src/cudd/2.3.0/cudd/libcudd.la \
	$(top_builddir)/src/cudd/2.3.0/util/libutil.la \
	$(top_builddir)/src/cudd/2.3.0/mtr/libmtr.la \
	$(top_builddir)/src/cudd/2.3.0/st/libst.la \
	-lsystemc

libscv_la_LDFLAGS = \
	-L$(SYSC_LIB_PREFIX) \
	$(LIBTOOL_LDFLAGS) \
	$(LIBTOOL_LDFLAGS_RELEASE)
