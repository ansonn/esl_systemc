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


LINARO_HOME=../Images/Linux/13.06/

RTSM_BOOTWRAPPER=$LINARO_HOME/rtsm/linux-system-semi.axf
RTSM_MMC=$LINARO_HOME/linaro.img
RTSM_KERNEL=$LINARO_HOME/uImage
RTSM_DTB=$LINARO_HOME/rtsm/rtsm_ve-ca15x2.dtb
RTSM_INITRD=$LINARO_HOME/uInitrd
RTSM_CMDLINE="console=ttyAMA0,115200n8 root=/dev/mmcblk0p2 rootwait ro mem=1024M ip=dhcp"	


$RTSM_MODEL \
	$RTSM_BOOTWRAPPER \
	-C motherboard.smsc_91c111.enabled=1 \
	-C motherboard.hostbridge.userNetworking=1 \
	-C motherboard.mmc.p_mmc_file="$RTSM_MMC" \
	-C cluster.cpu0.semihosting-cmd_line="--kernel $RTSM_KERNEL --dtb $RTSM_DTB --initrd $RTSM_INITRD -- $RTSM_CMDLINE"
