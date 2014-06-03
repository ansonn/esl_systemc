#! /bin/sh
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

LOGFILE1=$1
LOGFILE2=$2
GOLDENLOGPREFIX=$3

GOLDENLOGFILE1=${GOLDENLOGPREFIX}1.au
GOLDENLOGFILE2=${GOLDENLOGPREFIX}2.au
DIFFFILE="logfile.diff"
TMPFILE="logfile.grep"

if [ ! -f $LOGFILE1 ] || [ ! -f $LOGFILE2 ] || [ ! -f $GOLDENLOGFILE1 ] || [ ! -f $GOLDENLOGFILE2 ] ; then
    echo "fail - missing $LOGFILE1, $LOGFILE2, $GOLDENLOGFILE1 or $GOLDENLOGFILE2"
    exit
fi
grep "^%%REG: " $LOGFILE1 >$TMPFILE
if [ ! -f $TMPFILE ] ; then
    echo "fail - missing $TMPFILE"
    exit
fi
diff $TMPFILE $GOLDENLOGFILE1 >$DIFFFILE
rm $TMPFILE
if [ -s $DIFFFILE ] ; then
    echo "fail"
    exit
fi
grep "^%%REG: " $LOGFILE2 >$TMPFILE
if [ ! -f $TMPFILE ] ; then
    echo "fail - missing $TMPFILE"
    exit;
fi
diff $TMPFILE $GOLDENLOGFILE2 >$DIFFFILE
rm $TMPFILE
if [ -s $DIFFFILE ] ; then
    echo "fail"
else
    echo "passed"
fi
