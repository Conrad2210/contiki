# -*- coding: utf-8 -*-

import serial
import sys
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
        
    def serialWrite(self,output):
        self.serialPort.write(str(output) + "\n")
        
    def serialRead(self):
        out = ''
        while self.serialPort.inWaiting() > 0:
            out =+ self.serialPort.read(1)
            
        return out
        

        
    
    
    
