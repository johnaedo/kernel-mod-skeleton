#!/bin/bash

sudo dmesg -C
if lsmod | grep -wq "charkmod"; then
  		echo "charkmod is loaded. Unloading charkmod."
		sudo rmmod charkmod
fi
echo "charkmod is not loaded.  Loading charkmod.ko."
sudo insmod charkmod.ko
sudo chmod 777 /dev/charkmod
exit 0
