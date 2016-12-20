from PyQt4 import QtGui
from cycler import cycler
from PyQt4.QtCore import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
import databaseClassi
import serialCommunication
import thread

class MainWindow(QtGui.QMainWindow):    #The MainWindow is the core of the graphical interface

    output = ""
    def __init__(self):
        super(MainWindow, self).__init__()
        self.db = databaseClassi.databaseConnection()
        self.serial = serialCommunication.serialCommunication();
        
        self.receivingThread = thread
        self.receivingThreadID = 0
        self.initUI()

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
        number_label = QtGui.QLabel()                                #The following declaration serves for the TEST part of the window
        number_label.setText("# MSG's per Burst:")        
        self.number_text = QtGui.QLineEdit()
        
        neighConnected_label = QtGui.QLabel();
        neighConnected_label.setText(("Neighbours Connected: {0}").format(0))
        
        burst_label = QtGui.QLabel()                                    
        burst_label.setText("# Bursts:")        
        self.burst_text = QtGui.QLineEdit()
        
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
        
        for string in self.serial.serial_ports():
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
        
        test_HBox21.addWidget(burst_label)
        test_HBox21.addStretch(1)    
        test_HBox21.addWidget(self.burst_text)
        test_HBox21.addStretch(1)            
        test_HBox22.addWidget(interval_label)
        test_HBox22.addStretch(1)    
        test_HBox22.addWidget(self.interval_combo)
        test_HBox22.addStretch(1)    
        
        test_VBox1.addLayout(test_HBox21)
        test_VBox1.addLayout(test_HBox22)
        
        test_HBox23.addWidget(number_label)
        test_HBox23.addStretch(1)    
        test_HBox23.addWidget(self.number_text)        
        
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
        
        self.test_thread = Test_Worker()                                
        self.connect(self.test_thread, SIGNAL("plt_thread_launch(PyQt_PyObject)") ,self.plt_thread.launching)
        self.connect(self.test_thread, SIGNAL("experiments_combo_update()") ,self.experiments_combo_update)
        self.connect(self.test_thread, SIGNAL("list_update(PyQt_PyObject)") ,self.list_update)
        self.connect(self.test_thread, SIGNAL("status_bar_print(PyQt_PyObject)") ,self.status_bar_print)
        
        self.out_thread = Out_Worker(self.output)
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
        self.serial.closeConnection()
        self.CONNECT_button.clicked.connect(self.connectSerial)
        self.CONNECT_button.setText("Connect")
        
        
    def connectSerial(self):
        self.serial.openConnection(self.serialPorts_combo.currentText())        
        self.CONNECT_button.clicked.connect(self.closeSerial)
     
        
        self.CONNECT_button.setText("Close")
    
    def STOP(self) :                        #When the user presses the STOP button, it sends a burst_stop order to the test_thread
        self.test_thread.burst_stop_order()
     
    def START (self) :                        #Execute a serie of CAST with defined parameters
        temp = 0
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


class serialRead_Worker(QThread):
    def __init__(self, output,parent = None):

        QThread.__init__(self, parent)
        self.stop = 0
          
class Out_Worker(QThread):                            #Out_Worker is the Qthread responsible for the update of the ser_out text 

    def __init__(self, output,parent = None):

        QThread.__init__(self, parent)
        self.stop = 0
        self.output = output
        
    def setOutput(out):
        self.output.append(out)

    def run(self):                                    #The run method is the method called as a thread
        
        
        while 1 :
            if len(self.output) != 0 :
                self.emit(SIGNAL("output(PyQt_PyObject)"), self.output[0])
                self.output.remove(self.output[0])
                
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
        
class Test_Worker(QThread):                            #Test_Worker is the Qthread responsible for the running of the experiments
                                                    #It is called at the end of the TEST function and is there to time the different steps of an experiement
    def __init__(self, parent = None):                #This way, the time.sleep() commands don't freeze the main thread which is the whole window.
                                                    #It acts the same way as the end of the old TEST function
        QThread.__init__(self, parent)
        self.experiment_name = ""
        self.burst_number = 0
        self.cast_number = 0
        self.interval = 0
        self.rebroadcast = 0
        self.node_number = 0
        self.stop = 0
        self.burst_stop = 0

    def run(self):
    
        global node_list 
        global name_labels
        global burst
        global delay_queue
        global RQCT_send 
        global RQDT_received
        global CAST_end                                #CAST_end is a global flag used to tell this thread that the store thread has received the last CAST of the burst
                                                    #This way, the Test thread can wait for the last CAST to begin the RQDT sequence.
        self.plr_y = []                                #These two variable link the data of the process with the other threads (including the main)
        temp_plr_y = []
        delay_queue = []
        CAST_end = 0                                #This flag is modified in the store() process to show that all the CASTs have been send for this burst
        RQCT_send = 0
        RQDT_received = 0
        
        nodes_time = datetime.datetime.now() - datetime.datetime.now()
        nodes_time_mean = 0.0
        node_time = datetime.datetime.now() - datetime.datetime.now()
        node_time_mean = 0.0
        cast_time = datetime.datetime.now() - datetime.datetime.now()
        cast_time_mean = 0.0
        burst_time = datetime.datetime.now() - datetime.datetime.now()
        burst_time_begin = datetime.datetime.now()
        burst_time_mean = 0.0
        timeout = 120                                #The timeout variable sets a limit in sec after which the behaviour of the loop changes
        timeout_count = 0                            #timeout_count is the variable incremented every second in the while loops
        self.burst_stop = 0                            #burst_stop is a flag used to send a stop message to the test thread.
        burst_stopped = 0                            #When received, it stops the experiement at the end of the current burst
                                                    #If the stop order is sent before the end of the CASTs, it stop the CASTs and finishes right away
        for j in range(self.burst_number) :
            timeout_count = 0
            burst_time_begin = datetime.datetime.now()
#            output.append("---------------Begining of burst"+str(j + 1)+"!----------------")
            self.status_bar_print("----------Burst {} processed, cast time : {}, mean : {}, node time : {}, mean : {} , nodes time : {}, mean : {} , burst time : {}, mean : {}----------".format(j + 1, cast_time, cast_time_mean, node_time, node_time_mean, nodes_time, nodes_time_mean , burst_time, burst_time_mean))
            cast_time = datetime.datetime.now()
            input = ("CAST"+self.cast_number+"|30|"+self.interval+"|"+self.rebroadcast+"|00")
            
#            ser.write(input)
            while CAST_end == 0 :                    #While the network is waiting for a CAST, the process wait
                                                    #But if a burst_order is sent, the while breaks and we modifies the values of the burst_number
                if self.burst_stop == 1 :            #so that it is the last burst succesfully processed that counts
                    print("\nTest stopped")
                    burst_stopped = 1
#                    db_lock.acquire()                                #Accessing to the database : the lock is acquired.
#                    db = sqlite3.connect('Backup\\MyDb.db')
#                    cursor = db.cursor()
#                    cursor.execute("UPDATE tables set burst_number = "+str(j)+" WHERE experiment_name=?""", (self.experiment_name, ))
#                    db.commit()
#                    db.close()
#                    db_lock.release()
                    break
                if timeout_count == timeout :        #If we can't ensure every message is sent, the loop breaks
                        print "Packets missing, timeout achieved"
                        break
                time.sleep(1)
                timeout_count += 1
                
#            cast_time = datetime.datetime.now() - cast_time
#            cast_time_mean = float(cast_time_mean * (j) + cast_time.total_seconds() ) / float(j + 1)
#            self.status_bar_print("----------Burst {} in process, cast time : {}, mean : {}, node time : {}, mean : {} , nodes time : {}, mean : {} , burst time : {}, mean : {}----------".format(j + 1, cast_time, cast_time_mean, node_time, node_time_mean, nodes_time, nodes_time_mean , burst_time, burst_time_mean))
#            
#            if burst_stopped == 1 :
#                break
#            time.sleep(1)
#            print "---------CAST_end received !!---------"
#            time.sleep(30)
#            CAST_end = 0
#            for i in range(0,self.node_number) :
#                nodes_time = datetime.datetime.now() 
#                node_time = datetime.datetime.now() 
#                print"Beginning of the RQCT/RQDT sequence !!! for node {} number {}".format(node_list[i], i )
#                timeout_count = 0
#                RQCT_send = 1
#                time.sleep(2)
#                ser.write("RQCT" + node_list[i] + "0")
#                time.sleep(2)
#                ser.write("RQDT" + node_list[i] + "0")
#                print"waiting for RQDT_received for {}".format(node_list[i])
#                while RQDT_received == 0 :
#                    if timeout_count == timeout/2 :
#                        print "Node {} not answering !"
#                        # print"Asking again !"
#                        # timeout_count = timeout/2
#                        # RQCT_send = 1
#                        # time.sleep(0.5)
#                        # ser.write("RQCT" + node_list[i] + "0")
#                        # time.sleep(0.5)
#                        # ser.write("RQDT" + node_list[i] + "0")
#                        break
#                    time.sleep(1)
#                    timeout_count += 1
#                print "Datas from node {} received !".format(node_list[i])
#                RQDT_received = 0
#                RQCT_send = 0
#                time.sleep(1)
#                node_time = datetime.datetime.now() - node_time
#                node_time_mean = float(node_time_mean * (j) + node_time.total_seconds() ) / float((j + 1) * (i + 1))
#                self.status_bar_print("----------Burst {} in process, cast time : {}, mean : {}, node time : {}, mean : {} , nodes time : {}, mean : {} , burst time : {}, mean : {}----------".format(j + 1, cast_time, cast_time_mean, node_time, node_time_mean, nodes_time, nodes_time_mean , burst_time, burst_time_mean))
#            nodes_time = datetime.datetime.now() - nodes_time
#            nodes_time_mean = float(nodes_time_mean * (j) + nodes_time.total_seconds() ) / float((j + 1))
#            self.status_bar_print("----------Burst {} in process, cast time : {}, mean : {}, node time : {}, mean : {} , nodes time : {}, mean : {} , burst time : {}, mean : {}----------".format(j + 1, cast_time, cast_time_mean, node_time, node_time_mean, nodes_time, nodes_time_mean , burst_time, burst_time_mean))
#            time.sleep(2)
#            output.append("-----Burst "+str(j + 1)+" acquired-----")
#            temp_plr_y.append(burst_processing(self.experiment_name, j))
#            name_labels.append(self.experiment_name)
#            self.plr_y = [sum(temp_plr_y)/len(temp_plr_y)]    
#            self.list_update()
#            
#            self.status_bar_print("----------Burst {} processed, cast time : {}, mean : {}, node time : {}, mean : {} , nodes time : {}, mean : {} , burst time : {}, mean : {}----------".format(j + 1, cast_time, cast_time_mean, node_time, node_time_mean, nodes_time, nodes_time_mean , burst_time, burst_time_mean))
#            
#            self.plt_thread_launch(delay_queue)
#            burst +=1
#            if self.burst_stop == 1 :                #if the stop_order is received after the RQDT sequence beginning, the last process will be the one currently processed        
#                print("\nTest stopped")
#                db_lock.acquire()                                    #Accessing to the database : the lock is acquired.
#                db = sqlite3.connect('Backup\\MyDb.db')
#                cursor = db.cursor()
#                cursor.execute("UPDATE tables set burst_number = "+str(j+1)+" WHERE experiment_name=?""", (self.experiment_name, ))
#                db.commit()
#                db.close()
#                db_lock.release()
#                break
#            time.sleep(60)
#            if self.burst_stop == 1 :                #if the stop_order is received after the RQDT sequence beginning, the last process will be the one currently processed        
#                print("\nTest stopped")
#                db_lock.acquire()                                    #Accessing to the database : the lock is acquired.
#                db = sqlite3.connect('Backup\\MyDb.db')
#                cursor = db.cursor()
#                cursor.execute("UPDATE tables set burst_number = "+str(j+1)+" WHERE experiment_name=?""", (self.experiment_name, ))
#                db.commit()
#                db.close()
#                db_lock.release()
#                break
#            burst_time = datetime.datetime.now() - burst_time_begin
#            burst_time_mean = float(burst_time_mean * (j) + burst_time.total_seconds() ) / float(j + 1)
#            self.status_bar_print("----------Burst {} processed, cast time : {}, mean : {}, node time : {}, mean : {} , nodes time : {}, mean : {} , burst time : {}, mean : {}----------".format(j + 1, cast_time, cast_time_mean, node_time, node_time_mean, nodes_time, nodes_time_mean , burst_time, burst_time_mean))
#            
#        self.experiments_combo_update()
#        self.status_bar_print("----------Ready !cast time : {}, mean : {}, node time : {}, mean : {} , nodes time : {}, mean : {} , burst time : {}, mean : {}----------".format( cast_time, cast_time_mean, node_time, node_time_mean, nodes_time, nodes_time_mean , burst_time, burst_time_mean))
#        delay_queue = []
#
#        if self.stop == 1 :
#            print("\nTest thread ended")
#            self.terminate()
                                                                        #The launching method helps sending data to the thread before starting it
    def launching (self, experiment_name , burst_number , cast_number , interval , node_number) :
        self.experiment_name = experiment_name
        self.burst_number = burst_number
        self.cast_number = cast_number
        self.interval = interval
        self.node_number = node_number
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
