#!/bin/bash -e
#
#

redoeverything="a"
continueFlashing="a"
while [ "$redoeverything" != "n" ] ;
do
	sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=1,DEBUG_PRINT=1,LPDEVICE=0,EXTERNAL_LED=1 demonstrator
	continueFlashing="a"
	while [ "$continueFlashing" != "n" ] ;
	do
		sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=1,DEBUG_PRINT=1,EXTERNAL_LED=1 PORT=/dev/ttyUSB0 demonstrator.upload
		echo -n "ReDo Flashing [y/n]: "
		read continueFlashing
	done
	continueFlashing="a"
	while [ "$continueFlashing" != "n" ] ;
	do
		sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,EXTERNAL_LED=1 PORT=/dev/ttyUSB1 demonstrator.upload
		echo -n "ReDo Flashing [y/n]: "
		read continueFlashing
	done
	continueFlashing="a"
	while [ "$continueFlashing" != "n" ] ;
	do
		sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,LPDEVICE=1,EXTERNAL_LED=1 PORT=/dev/ttyUSB2 demonstrator.upload
		echo -n "ReDo Flashing [y/n]: "
		read continueFlashing
	done
	continueFlashing="a"
	while [ "$continueFlashing" != "n" ] ;
	do
		sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,EXTERNAL_LED=1 PORT=/dev/ttyUSB3 demonstrator.upload
		echo -n "ReDo Flashing [y/n]: "
		read continueFlashing
	done
echo -n "ReDo Everything [y/n]: "
read redoeverything
done


