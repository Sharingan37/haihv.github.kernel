#!/bin/bash

# Make sure only root can run our script
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

# Remove the existing xdma kernel module
lsmod | grep spidev
if [ $? -eq 0 ]; then
   rmmod spidev
fi
lsmod | grep bkav
if [ $? -eq 0 ]; then
   rmmod bkav
   rmmod bkav_spi
fi

echo -n "Loading driver..."
# Use the following command to Load the driver in the default 
# or interrupt drive mode. This will allow the driver to use 
# interrupts to signal when DMA transfers are completed.
insmod ../bkav_spi/bkav_spi.ko
insmod ../driver_spi/bkav.ko
# Use the following command to Load the driver in Polling
# mode rather than than interrupt mode. This will allow the
# driver to use polling to determ when DMA transfers are 
# completed.
#insmod ../driver/xdma.ko poll_mode=1


echo " DONE"
