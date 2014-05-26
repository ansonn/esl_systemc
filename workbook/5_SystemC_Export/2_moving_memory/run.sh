#!/bin/sh

/usr/bin/time ./Dhrystone2.x ${PVLIB_HOME}/images/dhrystone.axf < in.txt $*

