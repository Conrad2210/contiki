# -*- coding: utf-8 -*-

import serial

class serialCommunication():
    serialPort = {}
#    serial.Serial(
#    port='COM3',
#    baudrate=115200,
#    parity=serial.PARITY_NONE,
#    stopbits=serial.STOPBITS_ONE,
#    bytesize=serial.EIGHTBITS
#    )

    def openConnection(self,comPort):
        self.serialPort =  serial.Serial(
        port=str(comPort),
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS
        )
        
    def closeConnection(self):
        self.serialPort.close()

