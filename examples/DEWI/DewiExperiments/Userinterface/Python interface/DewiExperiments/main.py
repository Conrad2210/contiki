#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import time
import numpy
import serial
import thread
import sqlite3
import datetime
import threading
from PyQt4 import QtGui
from cycler import cycler
from PyQt4.QtCore import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
import databaseClassi
#configures the serial connections (the parameters differ depending on the device you are connected to)
ser = serial.Serial(
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

db_lock = thread.allocate_lock()                #This lock serves as a MUTEX to prevent the different thread to access the database simultaneously

experiment_name = ""                            #A global declaration used by the store the TEST function to give the experiment name to the store process

#This Script consists in three main threads.
#One to receive data and select which should be store.
#Another one to store the data selected in the database.
#The last one is the PyQt4 interface that permit the communication with the device and the management of the tests.

        
def receiving () :                            #receiving prints the informations received on the serial port
    print("Receiving started!")                
    global node_list                                #node_list contains the list of the shortadr of each registered node 
    global output                                    #output serves as a FIFO buffer for the serial_output to be print in the pyQt window
    global data                                        #data serves as a FIFO buffer for the store thread
    
    try :
        temp = ser.read(1)                    #temp is the temporary character read from the serial port
        msg_end = 0
        while 1 :
            out = ''                        #out stores the string read from the ser.read
            useful = 0                        #useful helps selecting which part of the string read we want to keep in the list (the shortadr)
            test = 0                        #test helps us determine which which string read we want to store as a data                                                
            elem = ""                        #elem is the string selected to be a part of the list or the data from the string read on the serial port
            
            while ser.inWaiting() > 0:
                if temp == '\n':
                    temp = ser.read(1)
                    break             
                if temp == "" :                                        # marks the end of the parameters of an answer
                    if test == 1 :                                        #We need to skip some characters (0\n) in order to properly stop to read 
                        test = 2
                    msg_end = 1
                    temp = ser.read(1)
                    break
                if temp == '0' and msg_end == 1 :
                    msg_end = 0
                    while temp != "":
                        temp = ser.read(1)
                    break 
                        
                if out == "DING" or out == "RESP" or out == "ROUT" or out == "DONG":    #Here we select which answer gives us the shortadr of a node
                    useful = 1
                if out == "DATA" or out == "CAST" or out == "DATC"  :                    #Here we select which answer are to be stored in the data.txt file
                    test = 1
                if temp == "|" :                                                #We don't need to have more than the first parameter the store the shortadr
                    useful = 0
                if useful == 1 or test == 1:
                    elem += temp
                out += temp    
                temp = ser.read(1)
                
            if out != '' :
                if out.find('\n') == -1 :                                                        #As long as something is read, we havee to print it.
                    output.append(out)                
    except :
        print "Receiveing terminated"                                                                            #------------------PRINT------------------
        return 
    

    
                                                    #The database_init fonction creates a table in the database named experiment_name, removed the previous 
                                                    #with the same name and also creates the tables table if it doesn't exist yet
        
                    #The Worker classes aree used by the PyQt window to create threads
                    #They are either working for ever, like the Out_Worker or are called at a precise moment when needed.                    
class Out_Worker(QThread):                            #Out_Worker is the Qthread responsible for the update of the ser_out text 

    def __init__(self, parent = None):

        QThread.__init__(self, parent)
        self.stop = 0

    def run(self):                                    #The run method is the method called as a thread
         global output
        
         while 1 :
             if len(output) != 0 :
                 self.emit(SIGNAL("output(PyQt_PyObject)"), output[0])
                 print output[0]
                 output.remove(output[0])
                
                 if self.stop == 1 :
                     print("\nOutput thread ended")
                     self.terminate()
    
    def stop_order(self):
        self.stop = 1
        
class Plt_Worker(QThread):                            #Plt_Worker is the Qthread responsible for the update of the different axes used to plot the datas 
                                                    #It is called at the end of each burst_processing session
    def __init__(self, parent = None):

        QThread.__init__(self, parent)
        self.stop = 0
        self.delay = []
        

    def run(self):
        
        print("\nUpdating the graph...\n")
        self.delay = sorted(self.delay)
        self.window_reset()
        self.latency_draw(self.delay)

        if self.stop == 1 :
            print("\nPlt thread ended")
            self.terminate()
    
    def latency_draw (self, data) :                #Draws the current data list in order to make it easier to have latency info
        self.emit(SIGNAL("latency_draw(PyQt_PyObject)"), data)
    
    def window_reset(self) :                    #Clears the current axes and reset the axis and title parameters
        self.emit(SIGNAL("window_reset()"))
    
    def launching (self, delay_queue) :            #The launching method helps sending data to the thread before starting it
        self.delay = delay_queue
        self.start()
    
    def stop_order(self):
        self.stop = 1
        
class Experiment_Worker(QThread):                            #Test_Worker is the Qthread responsible for the running of the experiments
                                                    #It is called at the end of the TEST function and is there to time the different steps of an experiement
    def __init__(self, parent = None):                #This way, the time.sleep() commands don't freeze the main thread which is the whole window.
                                                    #It acts the same way as the end of the old TEST function
        QThread.__init__(self, parent)
        self.experiment_name = ""
        self.numberBursts = 0
        self.interval = 0
        self.msgPerBurst = 0
    def run(self):
        global output
        global name_labels
        global burst
        global delay_queue                            #CAST_end is a global flag used to tell this thread that the store thread has received the last CAST of the burst
                                                    #This way, the Test thread can wait for the last CAST to begin the RQDT sequence.
        self.plr_y = []                                #These two variable link the data of the process with the other threads (including the main)
        temp_plr_y = []
        delay_queue = []
        ser.write("test string")
        output.append(("---------------Begin experiment: {0}!----------------").format(self.experiment_name))
        for j in range(self.numberBursts) :
            timeout_count = 0
            burst_time_begin = datetime.datetime.now()
            output.append("---------------Begining of burst"+str(j + 1)+"!----------------")
            
            for i in range(self.msgPerBurst):
                ser.write("Experiment\n")
                time.sleep(float(self.interval)/1000.0)
            
            print "time sleep: {0}".format(int((self.msgPerBurst * self.interval) / 1000) + 5)
            time.sleep(int((self.msgPerBurst * self.interval) / 1000) + 5)
            self.status_bar_print("----------Burst {0} of {1} processed----------".format(j+1, self.numberBursts))
            
        
        
        if self.stop == 1 :
            print("\nTest thread ended")
            self.terminate()
                                                                        #The launching method helps sending data to the thread before starting it
    def launching (self, experiment_name , numberBursts, interval , msgPerBurst) :
        self.experiment_name = experiment_name
        self.numberBursts = numberBursts
        self.interval = interval
        self.msgPerBurst = msgPerBurst
        self.start()
    
    def status_bar_print(self, str) :                                    #This method prints a message on the status bar
        self.emit(SIGNAL("status_bar_print(PyQt_PyObject)"), str)
    
    def experiments_combo_update(self) :                                #This method calls the experiments_combo_update method of the main window
        self.emit(SIGNAL("experiments_combo_update()"))
        
    def plt_thread_launch(self, delay_queue) :                            #This method launches the plt_thread from the main window
        self.emit(SIGNAL("plt_thread_launch(PyQt_PyObject)"), delay_queue)
        
    def list_update(self) :                                                #This method calls the list_update method of the main window
        self.emit(SIGNAL("list_update(PyQt_PyObject)"), self.plr_y)
    
    def burst_stop_order(self):                                            #This method is called when the user presses the STOP button
        self.burst_stop = 1                                                #It stops the waiting for the end of the Cast 
        self.status_bar_print("Waiting for the end of burst"+str(burst))#Or finishes the last RQDT sequence if received after the last CAST is received
        
    def stop_order(self):                                                #This method terminates the thread
        self.stop = 1
        self.status_bar_print("Ending the Test Thread")

class MainWindow(QtGui.QMainWindow):    #The MainWindow is the core of the graphical interface

    output = ""
    def __init__(self):
        super(MainWindow, self).__init__()
        self.db = databaseClassi.databaseConnection()
        self.initUI()

    def serial_ports(self):
        """ Lists serial port names
    
            :raises EnvironmentError:
                On unsupported or unknown platforms
            :returns:
                A list of the serial ports available on the system
        """
        if sys.platform.startswith('win'):
            ports = ['COM%s' % (i + 1) for i in range(256)]
        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this excludes your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')
        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')
        else:
            raise EnvironmentError('Unsupported platform')
    
        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result
    def initUI(self):                      #The initUI method initiates the whole graphical layout and start the output Qthread

        global delay_queue
        
        self.statusBar().showMessage('Ready !')    #The statusBar helps the user know what is going on and sends general and shows the progress of an experiement
                    
        self.xmax = 2000                            #xmax defines the highest horizontal lenght of the latency graph
        self.x_list = []                        #x_list is the list of the currently plot x_data for the the latency graph
        self.y_list = []                        #y_list is the list of the currently plot y_data for the the latency graph
        self.plr_x = []                            #x_list is the list of the currently plot x_data for the the plr graph
        self.plr_y = []                            #y_list is the list of the currently plot y_data for the the plr graph
        delay_queue = []
        
        self.figure_latency = plt.figure()                                        #Our figure, canvas and axes necessary to plot
        self.figure_plr = plt.figure()    
        self.canvas_latency = FigureCanvas(self.figure_latency)
        self.canvas_plr = FigureCanvas(self.figure_plr)
        self.axes_latency = self.figure_latency.add_subplot(111)
        self.axes_latency.hold(True)
        self.axes_plr = self.figure_plr.add_subplot(111)
        self.axes_plr.hold(True)
        # self.axes_plr.set_prop_cycle(cycler('color', ['c', 'm', 'y', 'k']))
        self.window_reset()
                                                                    #Then, a lot of declarations appears.
        msgPerBurst_label = QtGui.QLabel()                                #The following declaration serves for the TEST part of the window
        msgPerBurst_label.setText("# MSG's per Burst:")        
        self.msgPerBurst_text = QtGui.QLineEdit()
        
        neighConnected_label = QtGui.QLabel();
        neighConnected_label.setText(("Neighbours Connected: {0}").format(0))
        
        numberBursts_label = QtGui.QLabel()                                    
        numberBursts_label.setText("# Bursts:")        
        self.numberBursts_text = QtGui.QLineEdit()
        
        test_label = QtGui.QLabel()
        test_label.setText("Description: ")
        self.experiment_line = QtGui.QLineEdit()
        
        serialPort_label = QtGui.QLabel()
        serialPort_label.setText("Serial Port")
        
        interval_label = QtGui.QLabel()                                    
        interval_label.setText("Interarrivaltime [ms]:")        
        self.interval_combo = QtGui.QComboBox()
        self.interval_combo.addItem("50")
        self.interval_combo.addItem("100")
        self.interval_combo.addItem("200")
        self.interval_combo.addItem("300")
        
        self.serialPorts_combo = QtGui.QComboBox()
        
        for string in self.serial_ports():
            self.serialPorts_combo.addItem(string)
        
        self.compare1 = QtGui.QComboBox()
        self.compare2 = QtGui.QComboBox()
        self.compare3 = QtGui.QComboBox()
        self.compare4 = QtGui.QComboBox()
        self.compare5 = QtGui.QComboBox()
        
        self.check1 = QtGui.QCheckBox("1:")
        self.check2 = QtGui.QCheckBox("2:")
        self.check3 = QtGui.QCheckBox("3:")
        self.check4 = QtGui.QCheckBox("4:")
        self.check5 = QtGui.QCheckBox("5:")
        
        self.parameters1 = QtGui.QLabel()
        self.parameters2 = QtGui.QLabel()
        self.parameters3 = QtGui.QLabel()
        self.parameters4 = QtGui.QLabel()
        self.parameters5 = QtGui.QLabel()
        self.parameters1.setText("Title: | # Bursts : | Interarrivaltime : | number of node :  ")
        self.parameters2.setText("Title: | # Bursts : | Interarrivaltime : | number of node :  ")
        self.parameters3.setText("Title: | # Bursts : | Interarrivaltime : | number of node :  ")
        self.parameters4.setText("Title: | # Bursts : | Interarrivaltime : | number of node :  ")
        self.parameters5.setText("Title: | # Bursts : | Interarrivaltime : | number of node :  ")
        
        self.compare1.activated.connect(self.parameters_update1)
        self.compare2.activated.connect(self.parameters_update2)
        self.compare3.activated.connect(self.parameters_update3)
        self.compare4.activated.connect(self.parameters_update4)
        self.compare5.activated.connect(self.parameters_update5)
        
        START_button = QtGui.QPushButton('~  START  ~')
        STOP_button = QtGui.QPushButton('~  STOP  ~')
        PLOT_button = QtGui.QPushButton('PLOT')
                
        ser_out_label = QtGui.QLabel()                                #Then the Communication part
        ser_out_label.setText("Serial Output : ")
        self.ser_out = QtGui.QTextEdit()
        
        NeighUpdate_button = QtGui.QPushButton('Neigh Update')                            
        PRINT_button = QtGui.QPushButton('Print List')
        self.CONNECT_button = QtGui.QPushButton('Connect')

        START_button.clicked.connect(self.START)
        PLOT_button.clicked.connect(self.compare)                    #The buttons are then connected to their method
        NeighUpdate_button.clicked.connect(self.neighUpdate)                            
        PRINT_button.clicked.connect(self.printList)
        self.CONNECT_button.clicked.connect(self.connectSerial)
        
        top_widget = QtGui.QWidget()                                #Since the mainwindow is a MainWindow, it need a QWidget as a Central Widget
        top_Hbox = QtGui.QHBoxLayout()                                #The Layout is done with QVBoxLayout and QHBoxLayout
        
        
        comm_VBox = QtGui.QVBoxLayout()
        comm_HBox1 = QtGui.QHBoxLayout()
        comm_HBox2 = QtGui.QHBoxLayout()
        comm_HBox3 = QtGui.QHBoxLayout()
        

        
        comm_VBox.addLayout(comm_HBox1)
        comm_VBox.addLayout(comm_HBox2)
        comm_VBox.addLayout(comm_HBox3)
        comm_VBox.addWidget(ser_out_label)
        comm_VBox.addWidget(self.ser_out)
        
        comm_HBox1.addWidget(NeighUpdate_button)
        comm_HBox1.addWidget(PRINT_button)
        
        comm_HBox2.addWidget(neighConnected_label)        

        comm_HBox3.addWidget(serialPort_label)
        comm_HBox3.addWidget(self.serialPorts_combo)
        comm_HBox3.addWidget(self.CONNECT_button)

        
        test_VBox = QtGui.QVBoxLayout()
        test_HBox1 = QtGui.QHBoxLayout()
        test_HBox2 = QtGui.QHBoxLayout()
        test_HBox4 = QtGui.QHBoxLayout()
        test_HBox5 = QtGui.QHBoxLayout()
        test_HBox6 = QtGui.QHBoxLayout()
        test_HBox7 = QtGui.QHBoxLayout()
        test_HBox8 = QtGui.QHBoxLayout()
        test_HBox9 = QtGui.QHBoxLayout()
        test_HBox10 = QtGui.QHBoxLayout()
        test_HBox11 = QtGui.QHBoxLayout()
        
        test_HBox1.addStretch(1)
        test_HBox1.addWidget(test_label)
        test_HBox1.addWidget(self.experiment_line)
        test_HBox1.addStretch(1)

        test_HBox21 = QtGui.QHBoxLayout()
        test_HBox22 = QtGui.QHBoxLayout()
        test_HBox23 = QtGui.QHBoxLayout()
        test_HBox24 = QtGui.QHBoxLayout()
        test_VBox1 = QtGui.QVBoxLayout()
        test_VBox2 = QtGui.QVBoxLayout()
        
        test_HBox21.addWidget(numberBursts_label)
        test_HBox21.addStretch(1)    
        test_HBox21.addWidget(self.numberBursts_text)
        test_HBox21.addStretch(1)            
        test_HBox22.addWidget(interval_label)
        test_HBox22.addStretch(1)    
        test_HBox22.addWidget(self.interval_combo)
        test_HBox22.addStretch(1)    
        
        test_VBox1.addLayout(test_HBox21)
        test_VBox1.addLayout(test_HBox22)
        
        test_HBox23.addWidget(msgPerBurst_label)
        test_HBox23.addStretch(1)    
        test_HBox23.addWidget(self.msgPerBurst_text)        
        
        test_VBox2.addLayout(test_HBox23)
        test_VBox2.addLayout(test_HBox24)
        
        test_HBox2.addLayout(test_VBox1)
        test_HBox2.addLayout(test_VBox2)
        
        test_HBox4.addStretch(1)
        test_HBox4.addWidget(START_button)
        test_HBox4.addWidget(STOP_button)
        test_HBox4.addStretch(1)
        
        test_HBox5.addWidget(self.canvas_latency)
        test_HBox5.addWidget(self.canvas_plr)
        
        test_HBox6.addStretch(1)
        test_HBox6.addWidget(self.check1)
        test_HBox6.addWidget(self.compare1)
        test_HBox6.addWidget(self.parameters1)
        test_HBox6.addStretch(1)
        
        test_HBox7.addStretch(1)
        test_HBox7.addWidget(self.check2)
        test_HBox7.addWidget(self.compare2)
        test_HBox7.addWidget(self.parameters2)
        test_HBox7.addStretch(1)
        
        test_HBox8.addStretch(1)
        test_HBox8.addWidget(self.check3)
        test_HBox8.addWidget(self.compare3)
        test_HBox8.addWidget(self.parameters3)
        test_HBox8.addStretch(1)
        
        test_HBox9.addStretch(1)
        test_HBox9.addWidget(self.check4)
        test_HBox9.addWidget(self.compare4)
        test_HBox9.addWidget(self.parameters4)
        test_HBox9.addStretch(1)
        
        test_HBox10.addStretch(1)
        test_HBox10.addWidget(self.check5)
        test_HBox10.addWidget(self.compare5)
        test_HBox10.addWidget(self.parameters5)
        test_HBox10.addStretch(1)
        
        test_HBox11.addStretch(1)
        test_HBox11.addWidget(PLOT_button)
        test_HBox11.addStretch(1)
        
        test_VBox.addLayout(test_HBox1)
        test_VBox.addLayout(test_HBox2)
        test_VBox.addLayout(test_HBox4)
        test_VBox.addLayout(test_HBox5)
        test_VBox.addLayout(test_HBox6)
        test_VBox.addLayout(test_HBox7)
        test_VBox.addLayout(test_HBox8)
        test_VBox.addLayout(test_HBox9)
        test_VBox.addLayout(test_HBox10)
        test_VBox.addLayout(test_HBox11)
        
        
        top_Hbox.addLayout(comm_VBox)
        top_Hbox.addLayout(test_VBox)
        
        
        top_widget.setLayout(top_Hbox)
        self.setCentralWidget(top_widget) 
        
        self.setGeometry(40, 40, 1200, 650)
        self.setWindowTitle('Network testing')  

        self.plt_thread = Plt_Worker()                                    #Initialization different of the Qthreads
        self.connect(self.plt_thread, SIGNAL("window_reset()") ,self.window_reset)
        self.connect(self.plt_thread, SIGNAL("latency_draw(PyQt_PyObject)") ,self.latency_draw)
        
        self.experiment_thread = Experiment_Worker()                                
        self.connect(self.experiment_thread, SIGNAL("plt_thread_launch(PyQt_PyObject)") ,self.plt_thread.launching)
        self.connect(self.experiment_thread, SIGNAL("experiments_combo_update()") ,self.experiments_combo_update)
        self.connect(self.experiment_thread, SIGNAL("list_update(PyQt_PyObject)") ,self.list_update)
        self.connect(self.experiment_thread, SIGNAL("status_bar_print(PyQt_PyObject)") ,self.status_bar_print)
        
        self.out_thread = Out_Worker()
        self.connect(self.out_thread, SIGNAL("output(PyQt_PyObject)") ,self.output_update)
        self.out_thread.start()
        
        self.experiments_combo_update()
        
        self.show()
    
    def closeEvent(self, event):            #These are the methods to call when the window is being closed. The two Qthreads are asked to stop.
        self.out_thread.stop_order()
        self.plt_thread.stop_order()
    
    def keyPressEvent(self, event):            #If the 'ENTER' key is pressed, SEND the message
        key = event.key()

        if key == 16777220:
            self.SEND()
    
    def status_bar_print(self, str) :        #This method displays the str message on the statusBar
        self.statusBar().showMessage(str)
    
    def list_update(self, plr_mean) :        #This method is called via a signal by the test_thread to update values to plot the data 
        self.plr_y = [plr_mean]            
        self.x_list = []
        self.y_list = []
            
    def output_update (self, str) :            #This method update the ser_out text when receiveng a signal from the out_thread
        self.ser_out.append(str)
        c =  self.ser_out.textCursor()
        c.movePosition(QtGui.QTextCursor.End)
        self.ser_out.setTextCursor(c)
    
    def parameters_update1 (self) : 
        temp = 1    
           #The parameters_update methods display the parameter of each selected experiment
#        db_lock.acquire()
#        db = sqlite3.connect('Backup\\MyDb.db')
#        cursor = db.cursor()
#        
#        cursor.execute("""
#        CREATE TABLE IF NOT EXISTS tables(
#            experiment_name TEXT,
#            cast_number INTEGER,
#            burst_number INTEGER,
#            interval INTEGER,
#            rebroadcast INTEGER,
#            node_number INTEGER)""")
#        # cursor.execute("SELECT * FROM tables")
#        # for row in cursor :
#            # print row 
#        
#        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare1.currentText()) , ) )
#        for row in cursor :
#            self.parameters1.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
#        db.close()
#        db_lock.release()
        
    def parameters_update2 (self) :
        temp = 1
#        db_lock.acquire()
#        db = sqlite3.connect('Backup\\MyDb.db')
#        cursor = db.cursor()
#        
#        cursor.execute("""
#        CREATE TABLE IF NOT EXISTS tables(
#            experiment_name TEXT,
#            cast_number INTEGER,
#            burst_number INTEGER,
#            interval INTEGER,
#            rebroadcast INTEGER,
#            node_number INTEGER)""")
#        # cursor.execute("SELECT * FROM tables")
#        # for row in cursor :
#            # print row 
#        
#        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare2.currentText()) , ) )
#        for row in cursor :
#            self.parameters2.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
#        db.close()
#        db_lock.release()

    def parameters_update3 (self) :
        temp = 1
#        db_lock.acquire()
#        db = sqlite3.connect('Backup\\MyDb.db')
#        cursor = db.cursor()
#        
#        cursor.execute("""
#        CREATE TABLE IF NOT EXISTS tables(
#            experiment_name TEXT,
#            cast_number INTEGER,
#            burst_number INTEGER,
#            interval INTEGER,
#            rebroadcast INTEGER,
#            node_number INTEGER)""")
#        # cursor.execute("SELECT * FROM tables")
#        # for row in cursor :
#            # print row 
#        
#        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare3.currentText()) , ) )
#        for row in cursor :
#            self.parameters3.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
#        db.close()
#        db_lock.release()
        
    def parameters_update4 (self) :
        temp = 1
#        db_lock.acquire()
#        db = sqlite3.connect('Backup\\MyDb.db')
#        cursor = db.cursor()
#        
#        cursor.execute("""
#        CREATE TABLE IF NOT EXISTS tables(
#            experiment_name TEXT,
#            cast_number INTEGER,
#            burst_number INTEGER,
#            interval INTEGER,
#            rebroadcast INTEGER,
#            node_number INTEGER)""")
#        # cursor.execute("SELECT * FROM tables")
#        # for row in cursor :
#            # print row 
#        
#        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare4.currentText()) , ) )
#        for row in cursor :
#            self.parameters4.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
#        db.close()
#        db_lock.release()

    def parameters_update5 (self) :
        temp = 0
#        db_lock.acquire()
#        db = sqlite3.connect('Backup\\MyDb.db')
#        cursor = db.cursor()
#        
#        cursor.execute("""
#        CREATE TABLE IF NOT EXISTS tables(
#            experiment_name TEXT,
#            cast_number INTEGER,
#            burst_number INTEGER,
#            interval INTEGER,
#            rebroadcast INTEGER,
#            node_number INTEGER)""")
#        # cursor.execute("SELECT * FROM tables")
#        # for row in cursor :
#            # print row 
#        
#        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare5.currentText()) , ) )
#        for row in cursor :
#            self.parameters5.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
#        db.close()
#        db_lock.release()
    
    def experiments_combo_update (self) :    #Updates the items in the ComboBox that are used to select the experiments to plot
        experiment_list = []

##        cursor = db.cursor()
##        
##
##        for row in cursor :
##            experiment_list.append(str(row[0]))
##            
#        self.compare1.clear()
#        self.compare2.clear()
#        self.compare3.clear()
#        self.compare4.clear()
#        self.compare5.clear()
#            
#        self.compare1.addItems(experiment_list)
#        self.compare2.addItems(experiment_list)
#        self.compare3.addItems(experiment_list)
#        self.compare4.addItems(experiment_list)
#        self.compare5.addItems(experiment_list)
#        
#        print ("list of the different experiments updated : ")
#        print(experiment_list)
        
    def compare (self) :                    #Plot the data of the chosen experiments

        global delay_queue
        global name_labels                    #name_labels is a global list to pass the current experiement names to add a legend on the axes
        
        name_labels = []
        self.x_list = []
        self.y_list = []
        self.plr_x = []
        self.plr_y = []
        self.plr_y = []
        none_error = 0
        
        experiment_names = []                #this list serves as a storage for the selected experiments to plot
        if self.check1.isChecked() :
            experiment_names.append(str(self.compare1.currentText()))
        if self.check2.isChecked() :
            experiment_names.append(str(self.compare2.currentText()))
        if self.check3.isChecked() :
            experiment_names.append(str(self.compare3.currentText()))
        if self.check4.isChecked() :
            experiment_names.append(str(self.compare4.currentText()))
        if self.check5.isChecked() :
            experiment_names.append(str(self.compare5.currentText()))
        experiment_names = list(set(experiment_names))
                                            #for each experiment selected, the data are processed burst by burst
        # cast_time = datetime.datetime.now()
        for i in range(len(experiment_names)) :
            experiment_name = experiment_names[i]
            burst_count = self.get_burst_count(experiment_names[i])
            self.plr_x.append([])
            self.plr_y.append([])
            for j in range(burst_count) :
                print "Processing Burst number {} from ".format(j) + experiment_name                            #------------------PRINT------------------    
                self.plr_y[i].append(burst_processing(experiment_name, j))
                self.plr_x[i].append(j)

            self.plr_x[i] = [i]
            self.plr_y[i] = [sum(self.plr_y[i])/len(self.plr_y[i])]            #Replaces the list with the mean of its compomnents
            
            name_labels.append(experiment_name)                                #Adds the name of the experiment to the labels to show it on the bar graph
            self.plt_thread.launching(delay_queue)                            #The plotting thread is then called    
            delay_queue = []
            
        # cast_time =  datetime.datetime.now() - cast_time
        # print "PLOTTING DURATION : {}".format(cast_time)
    
    def get_burst_count (self, experiment_name) :        #This method gets the number of burst for an experiment in the tables table
        
        burst_count = 0
        
#        try : 
#            db_lock.acquire()
#            db = sqlite3.connect('Backup\\MyDb.db')
#            cursor = db.cursor()
#            cursor.execute("""SELECT burst_number FROM tables WHERE experiment_name=?""", (experiment_name,))
#            for row in cursor :
#                burst_count = row[0]
#            db.close
#            db_lock.release()
#        except sqlite3.Error as er:
#            print 'get burst er:', er.message
#        except Exception as e:
#            print(e)
        
        return burst_count
    def neighUpdate(self):
        temp = 0
        
    def printList(self):
        temp = 0
        
    def closeSerial(self):
        ser.close()
        self.CONNECT_button.clicked.connect(self.connectSerial)
        self.CONNECT_button.setText("Connect")
        
        
    def connectSerial(self):
        ser.port = self.serialPorts_combo.currentText()
        ser.open()
        thread.start_new_thread (receiving,( ))
        self.CONNECT_button.clicked.connect(self.closeSerial)       
        self.CONNECT_button.setText("Close")
    
    def STOP(self) :                        #When the user presses the STOP button, it sends a burst_stop order to the test_thread
        self.test_thread.burst_stop_order()
     
    def START (self) :                        #Execute a serie of CAST with defined parameters
        global burst							#burst is the number of the current burst (for the experiment)
        global output
        global experiment_name					#experiment_name is the name of the current experiment
        global delay_queue
        global name_labels						#name_labels is a global list to pass the current experiement names to add a legend on the axes
		
        name_labels = []
        temp_plr_y = []
        self.plr_x = [[0]]
        self.plr_y = [[]]
		
        try :
            burst_number = 0
            cast_number = ""
            interval = ""
            rebroadcast = "0"
            numberBursts = int(self.numberBursts_text.text())
            interval = int(self.interval_combo.currentText())
            msgPerBurst = int(self.msgPerBurst_text.text())
            burst = 0
            experiment_name = str(self.experiment_line.text())
            lastExperimentID = self.db.getLastExperimentID()
            newExperimentID = lastExperimentID + 1
            print "lastID: {0}, newID: {1}".format(lastExperimentID,newExperimentID)
            assert (experiment_name != "")
												#if not ebough information is provided, ends the test and print an error message
        except :																								#------------------PRINT------------------				
            print "INVALID INPUT"
            return 
            
        if experiment_name != "tables" :		#One cannot name his experiment tables. It would mess up the database organization... and wouldn't make much sens.
            #self.db.insertExperiment(newExperimentID,experiment_name,int(time.time()))
            #self.db.insertSettings(0, newExperimentID,0,numberBursts,int((msgPerBurst * interval) / 1000),msgPerBurst)
            self.experiment_thread.launching(experiment_name , numberBursts, interval , msgPerBurst)
        else:
           output.append("'tables' is a reserved name, use another one if you want to conduct an experiment.\n")		#------------------PRINT------------------	

    def latency_draw (self, data) :                #Draws the current data list in order to make it easier to have latency info
        
        if len(data) == 0 :
            return
        i = 0
        count = 0.0
        t = 0
        occurence_count = 1

        self.x_list.append([])
        self.y_list.append([])
        
        while t < self.xmax :
            if data[i] == t :
                count += 1.0
                self.update_line(data[i], count/(len(data)))
                if data.count(data[i]) == occurence_count :
                    t += 1
                    occurence_count = 1
                else :
                    occurence_count += 1
                if i != len(data) - 1 :
                    i += 1 
                
            else :
                self.update_line(t, count/(len(data)))
                t += 1
                if t == self.xmax :
                    self.update_line(t, count/(len(data)))
            
        self.plot()
        
    def update_line(self , new_x_data, new_y_data):    #Updates the current list of points to be drawn with x_data and y_data
        self.x_list[len(self.x_list)-1].append(new_x_data)
        self.y_list[len(self.y_list)-1].append(new_y_data)

    def plot (self):                                #Plots the contents of x_list and y_list and updates the canvas 
        x_99 = 0
        y_99 = 0.0
        y_200 = 0.0
        
        for i in range(len(self.x_list)) :
            if self.x_list[i] != [] :
                self.axes_latency.plot(self.x_list[i],self.y_list[i], linewidth=1 , label = name_labels[i])

        for i in range(len(self.x_list)) :
            if self.x_list[i] != [] :
                for j in range(len(self.x_list[i])) :
                    if self.y_list[i][j] >= 0.99 :
                        y_99 = self.y_list[i][j]
                        x_99 = self.x_list[i][j]
                        break
                for j in range(len(self.x_list[i])) :
                    if self.x_list[i][j] >= 200 :
                        y_200 = self.y_list[i][j]
                        break
                # self.axes_latency.plot([200 , 200],[0.0 , 2.0], label = "200ms : {0:.2f} %".format(y_200*100))                #Vertical + Horizontal
                # self.axes_latency.plot([0 , self.xmax],[y_99 , y_99], label = "99% : "+ str(x_99)+ " ms")
                # self.axes_latency.plot([0 , self.xmax],[y_200 , y_200], label = "200ms : {0:.2f} %".format(y_200*100))        #Horizontal + Vertical
                # self.axes_latency.plot([str(x_99) , str(x_99)],[0.0 , 2.0], label = "99% : "+ str(x_99)+ " ms")
                # self.axes_latency.plot([0 , 200 , 200],[y_200 , y_200, 0.0], label = "200ms : {0:.2f} %".format(y_200*100))    #Cross style
                # self.axes_latency.plot([0 , str(x_99) , str(x_99)],[y_99, y_99 , 0.0], label = "99% : "+ str(x_99)+ " ms")
                self.axes_latency.plot([200 , 200],[0.0 , 2.0], label = "200ms : {0:.2f} %".format(y_200*100))                    #Two Vertical 
                self.axes_latency.plot([str(x_99) , str(x_99)],[0.0 , 2.0], label = "99% : "+ str(x_99)+ " ms")
        
        self.axes_latency.legend(loc='lower right')
        
        for i in range(len(self.plr_x)) :
            self.axes_plr.bar(self.plr_x[i],self.plr_y[i])
            
        rects = self.axes_plr.patches                #To add labels on top of the bars to show the precise plr, rects and labels are used
        labels = []
        for i in range(len(self.plr_x)) :
            labels.append("{0:.2f} %".format(self.plr_y[i][0]))
        for rect, label, name_label in zip(rects, labels, name_labels):
            height = rect.get_height()
            self.axes_plr.text(rect.get_x() + rect.get_width()/2, height + 2, label, ha='center', va='bottom')
            self.axes_plr.text(rect.get_x() + rect.get_width()/2, height + 10, name_label, ha='center', va='bottom')
        
        self.canvas_latency.draw()
        self.canvas_plr.draw()
    
    def window_reset(self) :                        #Clears the current axes and reset the axis and title parameters
        self.axes_latency.cla()
        ymax = 1.05
        self.axes_latency.set_xlim([0,self.xmax])
        self.axes_latency.set_ylim([0,ymax])
        self.axes_latency.set_title("Percentage of received data versus time")
        self.axes_latency.set_xlabel("time (ms)")
        self.axes_latency.set_ylabel("Data received ")

        self.axes_plr.cla()
        max_burst = 1 
        for i in range(len(self.plr_x)) :
            if len(self.plr_x[i]) != 0 :
                if max(self.plr_x[i]) >= max_burst :
                    max_burst = max(self.plr_x[i]) + 1
        self.axes_plr.set_xlim([0, max_burst])
        self.axes_plr.set_ylim([0, 100])
        self.axes_plr.set_title("Package Loss Rate")
        self.axes_plr.set_xlabel("Experiment")
        self.axes_plr.set_ylabel("PLR (%)")
        
        self.canvas_latency.draw()
        self.canvas_plr.draw()
    
def data_backup(mode)    :                            #Saves the data list in 'mode' mode
    global data
    
    if (len(data) > 0) :
            data_file = open("C:\\Users\\R00145386\\Desktop\\clement.festal\\Notes\\Scripts\\Backup\\data.txt" , mode)
            data_file.write("\n------------------------\n")    
            data_file.write(datetime.datetime.now().strftime("%d/%m/%Y %Ih%Mm00")+"\n\n")
            for i in range(0,len(data)) :
                data_file.write (data[i]+"\n")
            data_file.close()
        
#MAIN FUNCTION

def main():

    try:
        global node_list
        global data
        global output                                #output is the buffer that will store the serial output while it's not printed in the window
        
        output = []
        node_list = []
        data = []
        i = 0                                        #------------------------------------------------------
                                    #This block is use to load the python list list[]
        elem = "elem"                                    #with the short adress of the devices inside                                                    


        
        ser.isOpen()
        
        app = QtGui.QApplication(sys.argv)
        mainwindow = MainWindow()
        

#        thread.start_new_thread (store,( ))
        thread.start_new_thread (sys.exit,(app.exec_() , ))
    
    finally :
    
        print("Closing before leaving!")                #When interrutping the programm, the list has to be saved in a file
        ser.close()    
        
if __name__ == '__main__':
    main()     







