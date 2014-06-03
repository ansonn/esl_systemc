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
## ****************************************************************************

SUBDIRS = doc

noinst_LTLIBRARIES = libcudd.la

EXTRA_DIST = \
	r7x8.1.mat \
	r7x8.1.out \
	testcudd.c 

libcudd_la_SOURCES = \
	cudd.h \
	cuddInt.h \
	cuddAPI.c \
	cuddAddAbs.c \
	cuddAddApply.c \
	cuddAddFind.c \
	cuddAddIte.c \
	cuddAddInv.c \
	cuddAddNeg.c \
	cuddAddWalsh.c \
	cuddAndAbs.c \
	cuddAnneal.c \
	cuddApa.c \
	cuddApprox.c \
	cuddBddAbs.c \
	cuddBddCorr.c \
	cuddBddIte.c \
	cuddBridge.c \
	cuddCache.c \
	cuddCheck.c \
	cuddClip.c \
	cuddCof.c \
	cuddCompose.c \
	cuddDecomp.c \
	cuddEssent.c \
	cuddExact.c \
	cuddExport.c \
	cuddGenCof.c \
	cuddGenetic.c \
	cuddGroup.c \
	cuddHarwell.c \
	cuddInit.c \
	cuddInteract.c \
	cuddLCache.c \
	cuddLevelQ.c \
	cuddLinear.c \
	cuddLiteral.c \
	cuddMatMult.c \
	cuddPriority.c \
	cuddRead.c \
	cuddRef.c \
	cuddReorder.c \
	cuddSat.c \
	cuddSign.c \
	cuddSolve.c \
	cuddSplit.c \
	cuddSubsetHB.c \
	cuddSubsetSP.c \
	cuddSymmetry.c \
	cuddTable.c \
	cuddUtil.c \
	cuddWindow.c \
	cuddZddCount.c \
	cuddZddFuncs.c \
	cuddZddGroup.c \
	cuddZddIsop.c \
	cuddZddLin.c \
	cuddZddMisc.c \
	cuddZddPort.c \
	cuddZddReord.c \
	cuddZddSetop.c \
	cuddZddSymm.c \
	cuddZddUtil.c

libcudd_la_LDFLAGS = \
	$(LIBTOOL_LDFLAGS)

libcudd_la_CPPFLAGS = \
	-I${top_srcdir}/src/cudd/2.3.0/util \
	-I${top_srcdir}/src/cudd/2.3.0/mtr \
	-I${top_srcdir}/src/cudd/2.3.0/st \
	-I${top_srcdir}/src \
	-I${top_builddir}/src
