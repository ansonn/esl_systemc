#!/bin/bash  

while getopts ":a:c:m:" opt; do
  case $opt in
    c)
      CORES=$OPTARG
      ;;
    a)
      CMD_ARGS=$OPTARG
      ;;
    m)
      RTSM_MODEL=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done    


LINARO_HOME=../Images/Linux/12.10/

RTSM_MODEL=./isim_Cortex-A15x2_VE_Linux-Debug-GCC-4.4
#$PVLIB_HOME/examples/RTSM_VE/Build_Cortex-A15x2/Linux64-Release-GCC-4.4/isim_system

RTSM_BOOTWRAPPER=$LINARO_HOME/boot/rtsm/linux-system-semi.axf
RTSM_MMC=$LINARO_HOME/linaro.img
RTSM_KERNEL=$LINARO_HOME/uImage
RTSM_DTB=$LINARO_HOME/boot/rtsm/rtsm_ve-ca15x2.dtb
RTSM_INITRD=$LINARO_HOME/filesystem-nano.cpio.gz
#$LINARO_HOME/uInitrd
RTSM_CMDLINE="console=ttyAMA0,115200n8 root=/dev/ram rdinit=/sbin/init rdinit=/sbin/init mem=1024M ip=dhcp"	


$RTSM_MODEL \
	$RTSM_BOOTWRAPPER \
	-C motherboard.smsc_91c111.enabled=1 \
	-C motherboard.hostbridge.userNetworking=1 \
	-C cluster.cpu0.semihosting-cmd_line="--kernel $RTSM_KERNEL --dtb $RTSM_DTB --initrd $RTSM_INITRD -- $RTSM_CMDLINE" 
