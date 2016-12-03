import serial
import time
import multiprocessing
import dewi as DBConnection

## Change this to match your local settings
SERIAL_PORT = '/dev/ttyUSB0'
SERIAL_BAUDRATE = 115200

class SerialProcess(multiprocessing.Process):

    dewi = DBConnection.dewi()
    addrMasterSlave = ""

    def __init__(self, input_queue, output_queue):
        multiprocessing.Process.__init__(self)
        self.input_queue = input_queue
        self.output_queue = output_queue
        self.sp = serial.Serial(SERIAL_PORT, SERIAL_BAUDRATE, timeout=1)

    def close(self):
        self.sp.close()

    def writeSerial(self, data):
        self.sp.write(data)
        # time.sleep(1)

    def readSerial(self):
        return self.sp.readline().replace("\n", "")

    def collectAddrMasterSlave(self,data):
        if 'MasterSlave_addr data:' in str(data):
            self.addrMasterSlave = data[data.index("'")+1:data.index("'",data.index("'")+1,len(data))]
            return True
        else:
            return False

    def toHex(s):
        lst = []
        for ch in s:
            hv = hex(ord(ch)).replace('0x', '')
            if len(hv) == 1:
                hv = '0'+hv
                lst.append(hv)

        return reduce(lambda x,y:x+y, lst)


    


    def sendSerialData(self,data):
        if "Experiments" in data:
            rawData = data[data.index("(")+1:data.index(")")]
            splitted = rawData.split(",")
            print len(splitted)
            id = splitted[0]
            description = str(splitted[1])
            date_time = splitted[2]
            print ("rawData: {0}, id: {1}, description: {2}, date_time: {3}").format(rawData,id, description,date_time)
            self.dewi.insertExperiment(id, description, date_time)
    
        if "Settings" in data:
            rawData = data[data.index("(")+1:data.index(")")]
            splitted = rawData.split(",")
            print len(splitted)
            id = splitted[0]
            session_id = splitted[1]
            txPower =  splitted[2]
            numBursts =  splitted[3]
            burstDuration =  splitted[4]
            msg =  splitted[5]
            self.dewi.insertSettings(id,session_id,txPower,numBursts,burstDuration,msg)
    
        if "Latency" in data:
            rawData = data[data.index("(")+1:data.index(")")]
            splitted = rawData.split(",")
            print len(splitted)
            id = splitted[0]
            session_id = splitted[1]
            nodeID =  splitted[2]
            timeslot =  splitted[3]
            count =  splitted[4]
            self.dewi.insertLatency(id,session_id,nodeID,timeslot,count)
    
        if "RxPackets" in data:
            rawData = data[data.index("(")+1:data.index(")")]
            splitted = rawData.split(",")
            print len(splitted)
            id = splitted[0]
            session_id = splitted[1]
            nodeID =  splitted[2]
            count =  splitted[3]
            self.dewi.insertRxPackets(id,session_id,nodeID,count)
    
        if "TxPackets" in data:
            rawData = data[data.index("(")+1:data.index(")")]
            splitted = rawData.split(",")
            print len(splitted)
            id = splitted[0]
            session_id = splitted[1]
            nodeID =  splitted[2]
            count =  splitted[3]
            self.dewi.insertTxPackets(id,session_id,nodeID,count)
    
#
 
            
    def sendToSerial(self,data):
        if "resetstatistics" in data:
            self.dewi.cleanStatsTable()
        if "topologyrefresh" in data:
            self.dewi.cleanNodeLinkTables()
            

    def run(self):

        self.sp.flushInput()

        while True:
            # look for incoming tornado request
            if not self.input_queue.empty():
                data = self.input_queue.get()
                self.sendToSerial(data)
                # send it to the serial device
                self.writeSerial(data)
                self.output_queue.put("writing to serial: " + data)
                print "writing to serial: " + data

            # look for incoming serial data
            if (self.sp.inWaiting() > 0):
                data = self.readSerial()
                self.sendSerialData(data)
                print "reading from serial: " + data
                # send it back to tornado
                self.output_queue.put(data)
    
