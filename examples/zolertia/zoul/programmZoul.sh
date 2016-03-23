#!bin/sh
sudo make TARGET=zoul zoul-demo 
sudo make zoul-demo.upload PORT=/dev/ttyUSB0 
sudo make zoul-demo.upload PORT=/dev/ttyUSB1 
sudo make zoul-demo.upload PORT=/dev/ttyUSB2 
sudo make zoul-demo.upload PORT=/dev/ttyUSB3 
