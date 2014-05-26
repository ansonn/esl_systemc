#!/bin/sh
#
# run.sh - Run the EVS_GlobalInterface example.
#
# Copyright 2012 ARM Limited.
# All rights reserved.
#

axf=hello.axf

if [ ! -e ${axf} ]; then
    echo "ERROR: ${axf}: application not found"
    exit 1
fi

./EVS_GlobalInterface_Cortex-A15x1.x  -a ${axf} -S <<EOF $*

EOF

