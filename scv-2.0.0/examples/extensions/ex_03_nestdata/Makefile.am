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
##  Process this file with automake to produce Makefile.in
##
## ****************************************************************************
##
##  Original Authors:
##  Eric Paire, Jerome Cornet
##  STMicroelectronics, 2012-04-27
##
## ****************************************************************************
##  
##  MODIFICATION LOG - modifiers, enter your name, affiliation, date and
##  changes you are making here.
##
##      Name, Affiliation, Date: 
##  Description of Modification:
##
## ****************************************************************************

TESTS = \
	ex_03_nestdata

check_PROGRAMS = \
	ex_03_nestdata

ex_03_nestdata_SOURCES = \
	state.h \
	state_ext.h \
	test.cc

EXTRA_DIST = \
	README \
	checktest.sh \
	ex_03_nestdata.vcproj \
	test_reg.cc


if SCV_INSTALL_BUILDDIR
examplesdir=$(prefix)/examples
else
examplesdir=$(prefix)/examples/scv
endif
my_datadir = $(examplesdir)/extensions/ex_03_nestdata

if !SCV_INSTALL_SOURCEDIR # do not duplicate examples in source tree

my_data_DATA = \
	$(EXTRA_DIST) \
	$(ex_03_nestdata_SOURCES)

endif # !SCV_INSTALL_SOURCEDIR

ex_03_nestdata_CPPFLAGS = \
	-I$(SYSC_INC_PREFIX) \
	-I$(top_srcdir)/src

ex_03_nestdata_LDADD = \
	$(top_builddir)/src/scv/libscv.la -lsystemc

ex_03_nestdata_LDFLAGS = \
	-R$(SYSC_LIB_PREFIX)
