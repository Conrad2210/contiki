#!bin/sh

sudo make TARGET=zoul PORT=/dev/ttyUSB0 test-tsch-coordinator
sudo make TARGET=zoul PORT=/dev/ttyUSB0 test-tsch-coordinator.upload


sudo make TARGET=zoul PORT=/dev/ttyUSB1 test-tsch
sudo make TARGET=zoul PORT=/dev/ttyUSB1 test-tsch.upload
sudo make TARGET=zoul PORT=/dev/ttyUSB2 test-tsch.upload
sudo make TARGET=zoul PORT=/dev/ttyUSB3 test-tsch.upload
