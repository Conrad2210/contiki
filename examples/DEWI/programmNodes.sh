#!/bin/bash -e
#
#

redoeverything="a"
continueFlashing="a"
while [ "$redoeverything" != "n" ] ;
do
		make PORT=/dev/ttyUSB0 demonstrator-coordinator.upload
		make PORT=/dev/ttyUSB1 demonstrator-node.upload
		make PORT=/dev/ttyUSB2 demonstrator-node.upload
		make PORT=/dev/ttyUSB3 demonstrator-node.upload
		make PORT=/dev/ttyUSB4 demonstrator-node.upload
echo -n "ReDo Everything [y/n]: "
read redoeverything
done


