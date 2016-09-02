#!/bin/bash -e
#
#

redoeverything="a"
continueFlashing="a"
while [ "$redoeverything" != "n" ] ;
do
		sudo make TARGET=zoul BOARD=remote  PORT=/dev/ttyUSB0 demonstrator-coordinator.upload
		sudo make TARGET=zoul BOARD=remote  PORT=/dev/ttyUSB1 demonstrator-node.upload
		sudo make TARGET=zoul BOARD=remote  PORT=/dev/ttyUSB2 demonstrator-node.upload
		sudo make TARGET=zoul BOARD=remote  PORT=/dev/ttyUSB3 demonstrator-node.upload
echo -n "ReDo Everything [y/n]: "
read redoeverything
done


