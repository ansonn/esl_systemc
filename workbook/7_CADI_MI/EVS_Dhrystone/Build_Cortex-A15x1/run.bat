@echo off
rem 
rem run.bat - Run the EVS_Dhrystone example.
rem 
rem Copyright 2012 ARM Limited.
rem All rights reserved.
rem 

set axf="%PVLIB_HOME%\images\dhrystone.axf"

if not exist %axf% (
    echo ERROR: %axf%: application not found
    echo Did you install the Fast Models TPIP package?
    goto end
)

echo 1000000 | .\EVS_Dhrystone_Cortex-A15x1.exe -a %axf% %*

:end
pause

