#!bin/sh

sudo make TARGET=zoul PORT=/dev/ttyUSB0 all
sudo make TARGET=zoul PORT=/dev/ttyUSB0 test_tsch_coordinator.upload

sudo make TARGET=zoul PORT=/dev/ttyUSB1 test-tsch.upload
sudo make TARGET=zoul PORT=/dev/ttyUSB2 test-tsch.upload
sudo make TARGET=zoul PORT=/dev/ttyUSB3 test-tsch.upload
