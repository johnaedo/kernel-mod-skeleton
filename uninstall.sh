#!/bin/bash

if lsmod | grep -wq "charkmod"; then
  		echo "charkmod is loaded.  Unloading charkmod."
		sudo rmmod charkmod
  		exit 0
	else
  		echo "charkmod is not loaded.  No action taken."
  	exit 0
fi
