#!/bin/bash -e
#
#

redoeverything="a"
continueFlashing="a"
while [ "$redoeverything" != "n" ] ;
do
		sudo make TARGET=zoul BOARD=remote  PORT=/dev/ttyUSB0 demonstrator-coordinator.upload
		sudo make TARGET=zoul BOARD=remote  PORT=/dev/ttyUSB1 demonstrator-node.upload
		#sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,EXTERNAL_LED=1 PORT=/dev/ttyUSB3 demonstrator.upload
		#sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,LPDEVICE=1,EXTERNAL_LED=1 PORT=/dev/ttyUSB2 demonstrator.upload
		#sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=1,EXTERNAL_LED=1 PORT=/dev/ttyUSB4 demonstrator.upload
echo -n "ReDo Everything [y/n]: "
read redoeverything
done


