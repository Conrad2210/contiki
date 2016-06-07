#!/bin/bash -e
#
#


continueFlashing="a"
while [ "$continueFlashing" != "n" ] ;
do
	sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=1,DEBUG_PRINT=1,LPDEVICE=0 PORT=/dev/ttyUSB0 demonstrator.upload
	echo -n "ReDo Flashing [y/n]: "
	read continueFlashing
done
continueFlashing="a"
while [ "$continueFlashing" != "n" ] ;
do
	sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,LPDEVICE=0 PORT=/dev/ttyUSB1 demonstrator.upload
	echo -n "ReDo Flashing [y/n]: "
	read continueFlashing
done
continueFlashing="a"
while [ "$continueFlashing" != "n" ] ;
do
	sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,LPDEVICE=1 PORT=/dev/ttyUSB2 demonstrator.upload
	echo -n "ReDo Flashing [y/n]: "
	read continueFlashing
done



