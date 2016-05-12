#!bin/sh

sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=1,DEBUG_PRINT=0,LPDEVICE=0 PORT=/dev/ttyUSB0 demonstrator.upload
sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=0,LPDEVICE=0 PORT=/dev/ttyUSB1 demonstrator.upload
sudo make TARGET=zoul BOARD=remote DEFINES+=ISCOORD=0,DEBUG_PRINT=0,LPDEVICE=1 PORT=/dev/ttyUSB2 demonstrator.upload
