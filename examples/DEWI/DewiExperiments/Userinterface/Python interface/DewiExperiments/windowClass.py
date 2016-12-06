from PyQt4 import QtGui
from cycler import cycler
from PyQt4.QtCore import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
import databaseClassi

class MainWindow(QtGui.QMainWindow):    #The MainWindow is the core of the graphical interface

    
    def __init__(self):
        super(MainWindow, self).__init__()
        self.db = databaseClassi.databaseConnection()
        self.initUI()

    def initUI(self):                      #The initUI method initiates the whole graphical layout and start the output Qthread
    
        global output
        global delay_queue
        
        self.setWindowIcon(QtGui.QIcon('C:\\Users\\R00145386\\Desktop\\clement.festal\\Notes\\Scripts\\Images\\network.png')) 
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
        number_label.setText("Number of message per Burst :")        
        self.number_text = QtGui.QLineEdit()
        
        burst_label = QtGui.QLabel()                                    
        burst_label.setText("Number of Burst for the experiment :")        
        self.burst_text = QtGui.QLineEdit()
        
        test_label = QtGui.QLabel()
        test_label.setText("Name of the Test : ")
        self.experiment_line = QtGui.QLineEdit()
        
        interval_label = QtGui.QLabel()                                    
        interval_label.setText("Interval between each message in ms:")        
        self.interval_combo = QtGui.QComboBox()
        self.interval_combo.addItem("50")
        self.interval_combo.addItem("100")
        self.interval_combo.addItem("200")
        self.interval_combo.addItem("300")
        self.interval_combo.addItem("400")
        self.interval_combo.addItem("500")
        self.interval_combo.addItem("1000")
        self.interval_combo.addItem("2000")
        
        rebroadcast_label = QtGui.QLabel()
        rebroadcast_label.setText("Ask nodes to rebroadcast :")        
        self.rebroadcast_combo = QtGui.QComboBox()
        self.rebroadcast_combo.addItem("no")
        self.rebroadcast_combo.addItem("yes")
        
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
        self.parameters1.setText("cast per message :  | burst : | interval :    | rebroadcast :  | number of node :  ")
        self.parameters2.setText("cast per message :  | burst : | interval :    | rebroadcast :  | number of node :  ")
        self.parameters3.setText("cast per message :  | burst : | interval :    | rebroadcast :  | number of node :  ")
        self.parameters4.setText("cast per message :  | burst : | interval :    | rebroadcast :  | number of node :  ")
        self.parameters5.setText("cast per message :  | burst : | interval :    | rebroadcast :  | number of node :  ")
        
        self.compare1.activated.connect(self.parameters_update1)
        self.compare2.activated.connect(self.parameters_update2)
        self.compare3.activated.connect(self.parameters_update3)
        self.compare4.activated.connect(self.parameters_update4)
        self.compare5.activated.connect(self.parameters_update5)
        
        TEST_button = QtGui.QPushButton('~  TEST  ~')
        STOP_button = QtGui.QPushButton('~  STOP  ~')
        PLOT_button = QtGui.QPushButton('PLOT')
                
        ser_out_label = QtGui.QLabel()                                #Then the Communication part
        ser_out_label.setText("Serial Output : ")
        self.ser_out = QtGui.QTextEdit()
        self.ser_in = QtGui.QLineEdit()
        
        LIST_button = QtGui.QPushButton('LIST')                            
        PING_button = QtGui.QPushButton('PING')
        SEND_button = QtGui.QPushButton('SEND')
        TIMA_button = QtGui.QPushButton('TIMA')
        RFRL_button = QtGui.QPushButton('RFRL')
        SYNC_button = QtGui.QPushButton('SYNC')
        
        PLOT_button.clicked.connect(self.compare)                    #The buttons are then connected to their method
        LIST_button.clicked.connect(self.LIST)                            
        PING_button.clicked.connect(self.PING)
        SEND_button.clicked.connect(self.SEND)
        TIMA_button.clicked.connect(self.TIMA)
        RFRL_button.clicked.connect(self.RFRL)
        SYNC_button.clicked.connect(self.SYNC)
        TEST_button.clicked.connect(self.TEST)
        STOP_button.clicked.connect(self.STOP)
        
        top_widget = QtGui.QWidget()                                #Since the mainwindow is a MainWindow, it need a QWidget as a Central Widget
        top_Hbox = QtGui.QHBoxLayout()                                #The Layout is done with QVBoxLayout and QHBoxLayout
        
        
        comm_VBox = QtGui.QVBoxLayout()
        comm_HBox1 = QtGui.QHBoxLayout()
        comm_HBox2 = QtGui.QHBoxLayout()
        comm_HBox3 = QtGui.QHBoxLayout()
        
        comm_HBox1.addWidget(self.ser_in)
        comm_HBox1.addWidget(SEND_button)
        
        comm_HBox2.addWidget(LIST_button)
        comm_HBox2.addWidget(TIMA_button)
        comm_HBox2.addWidget(PING_button)
        comm_HBox2.addWidget(RFRL_button)
        
        comm_HBox3.addStretch(1)
        comm_HBox3.addWidget(SYNC_button)
        comm_HBox3.addStretch(1)
        
        comm_VBox.addLayout(comm_HBox1)
        comm_VBox.addLayout(comm_HBox2)
        comm_VBox.addLayout(comm_HBox3)
        comm_VBox.addWidget(ser_out_label)
        comm_VBox.addWidget(self.ser_out)
        
        
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
        test_HBox23.addStretch(1)    
        test_HBox24.addWidget(rebroadcast_label)
        test_HBox24.addStretch(1)    
        test_HBox24.addWidget(self.rebroadcast_combo)
        test_HBox24.addStretch(1)    
        
        test_VBox2.addLayout(test_HBox23)
        test_VBox2.addLayout(test_HBox24)
        
        test_HBox2.addLayout(test_VBox1)
        test_HBox2.addLayout(test_VBox2)
        
        test_HBox4.addStretch(1)
        test_HBox4.addWidget(TEST_button)
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
    
    def parameters_update1 (self) :            #The parameters_update methods display the parameter of each selected experiment
        db_lock.acquire()
        db = sqlite3.connect('Backup\\MyDb.db')
        cursor = db.cursor()
        
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS tables(
            experiment_name TEXT,
            cast_number INTEGER,
            burst_number INTEGER,
            interval INTEGER,
            rebroadcast INTEGER,
            node_number INTEGER)""")
        # cursor.execute("SELECT * FROM tables")
        # for row in cursor :
            # print row 
        
        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare1.currentText()) , ) )
        for row in cursor :
            self.parameters1.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
        db.close()
        db_lock.release()
        
    def parameters_update2 (self) :
        db_lock.acquire()
        db = sqlite3.connect('Backup\\MyDb.db')
        cursor = db.cursor()
        
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS tables(
            experiment_name TEXT,
            cast_number INTEGER,
            burst_number INTEGER,
            interval INTEGER,
            rebroadcast INTEGER,
            node_number INTEGER)""")
        # cursor.execute("SELECT * FROM tables")
        # for row in cursor :
            # print row 
        
        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare2.currentText()) , ) )
        for row in cursor :
            self.parameters2.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
        db.close()
        db_lock.release()

    def parameters_update3 (self) :
        db_lock.acquire()
        db = sqlite3.connect('Backup\\MyDb.db')
        cursor = db.cursor()
        
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS tables(
            experiment_name TEXT,
            cast_number INTEGER,
            burst_number INTEGER,
            interval INTEGER,
            rebroadcast INTEGER,
            node_number INTEGER)""")
        # cursor.execute("SELECT * FROM tables")
        # for row in cursor :
            # print row 
        
        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare3.currentText()) , ) )
        for row in cursor :
            self.parameters3.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
        db.close()
        db_lock.release()
        
    def parameters_update4 (self) :
        db_lock.acquire()
        db = sqlite3.connect('Backup\\MyDb.db')
        cursor = db.cursor()
        
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS tables(
            experiment_name TEXT,
            cast_number INTEGER,
            burst_number INTEGER,
            interval INTEGER,
            rebroadcast INTEGER,
            node_number INTEGER)""")
        # cursor.execute("SELECT * FROM tables")
        # for row in cursor :
            # print row 
        
        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare4.currentText()) , ) )
        for row in cursor :
            self.parameters4.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
        db.close()
        db_lock.release()

    def parameters_update5 (self) :
        db_lock.acquire()
        db = sqlite3.connect('Backup\\MyDb.db')
        cursor = db.cursor()
        
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS tables(
            experiment_name TEXT,
            cast_number INTEGER,
            burst_number INTEGER,
            interval INTEGER,
            rebroadcast INTEGER,
            node_number INTEGER)""")
        # cursor.execute("SELECT * FROM tables")
        # for row in cursor :
            # print row 
        
        cursor.execute("SELECT cast_number, burst_number, interval, rebroadcast, node_number FROM tables WHERE experiment_name=?", (str(self.compare5.currentText()) , ) )
        for row in cursor :
            self.parameters5.setText("cast per message : "+ str(row[0]) +" | burst : "+ str(row[1]) +" | interval : "+ str(row[2]) +" | rebroadcast : "+ str(row[3]) +" | number of node : "+ str(row[4]))
        db.close()
        db_lock.release()
    
    def experiments_combo_update (self) :    #Updates the items in the ComboBox that are used to select the experiments to plot
        experiment_list = []
        db_lock.acquire()
        db = sqlite3.connect('Backup\\MyDb.db')
        cursor = db.cursor()
        
        cursor.execute("""
        CREATE TABLE IF NOT EXISTS tables(
            experiment_name TEXT,
            cast_number INTEGER,
            burst_number INTEGER,
            interval INTEGER,
            rebroadcast INTEGER,
            node_number INTEGER)""")
        
        cursor.execute("SELECT experiment_name FROM tables")
        for row in cursor :
            experiment_list.append(str(row[0]))
        db.close()
        db_lock.release()
            
        self.compare1.clear()
        self.compare2.clear()
        self.compare3.clear()
        self.compare4.clear()
        self.compare5.clear()
            
        self.compare1.addItems(experiment_list)
        self.compare2.addItems(experiment_list)
        self.compare3.addItems(experiment_list)
        self.compare4.addItems(experiment_list)
        self.compare5.addItems(experiment_list)
        
        print ("list of the different experiments updated : ")
        print(experiment_list)
        
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
        
        try : 
            db_lock.acquire()
            db = sqlite3.connect('Backup\\MyDb.db')
            cursor = db.cursor()
            cursor.execute("""SELECT burst_number FROM tables WHERE experiment_name=?""", (experiment_name,))
            for row in cursor :
                burst_count = row[0]
            db.close
            db_lock.release()
        except sqlite3.Error as er:
            print 'get burst er:', er.message
        except Exception as e:
            print(e)
        
        return burst_count
    
    def STOP(self) :                        #When the user presses the STOP button, it sends a burst_stop order to the test_thread
        self.test_thread.burst_stop_order()
    
    def RFRL (self) :                        #RFRL for Refresh list : clear the node_list and send a command to update it
        global node_list
        global output
        temp_list = node_list
        node_list = []
        time.sleep(1)
        for i in range(0,len(temp_list)) :
            output.append("PING" + temp_list[i] + "0")
            ser.write("PING" + temp_list[i] + "0")
            time.sleep(0.2)
        output.append("List refreshed !")                                                                        #------------------PRINT------------------                
    
    def SYNC (self) :                        #Give the same time for every node (precision only up to seconds) 
        ser.write("TIME0|"+datetime.datetime.now().strftime("%Y%m%d%I%M00")+"0")
    
    def LIST (self) :                        #prints the list of node registered
        output.append("---------------Begining of the list!----------------")
        for i in range(0,len(node_list)) :
            output.append("{} : ".format(i) + node_list[i])
        output.append("---------------Ending of the list!------------------")
    
    def PING (self) :                        #PING every node registered in the node_list
        output.append("---------------Begining of the PING sequence!----------------")
        for i in range(0,len(node_list)) :
            output.append("PING" + node_list[i] + "0")
            ser.write("PING" + node_list[i] + "0")
            time.sleep(0.2)
        
    def TIMA (self) :                        #QTIM every node registered in the list
        output.append("---------------Sending QTIM request!----------------")
        output.append("QTIM" + "0" + "0")
        ser.write("QTIM" + "0" + "0")
        time.sleep(0.2)
        for i in range(0,len(node_list)) :
            output.append("QTIM" + node_list[i] + "0")
            ser.write("QTIM" + node_list[i] + "0")
            time.sleep(0.2)
    
    def SEND (self) :                        #Send the message wrapped in the separators  0 via the serial connection 
        input = unicode(self.ser_in.text(), "utf-8")
        if input != "" :
            ser.write(""+input.encode()+"0"+"\n")
            output.append(""+input+"0"+"\n")
            self.ser_in.setText("")
    
    def TEST (self) :                        #Execute a serie of CAST with defined parameters
        global burst                            #burst is the number of the current burst (for the experiment)
        global output
        global experiment_name                    #experiment_name is the name of the current experiment
        global delay_queue
        global name_labels                        #name_labels is a global list to pass the current experiement names to add a legend on the axes
        
        name_labels = []
        temp_plr_y = []
        self.plr_x = [[0]]
        self.plr_y = [[]]
        
        try :
            burst_number = 0
            cast_number = ""
            interval = ""
            rebroadcast = "0"
            cast_number = str(self.number_text.text())
            burst_number = int(self.burst_text.text())
            interval = str(self.interval_combo.currentText())
            burst = 0
            experiment_name = str(self.experiment_line.text())
            print ("Experiment for the TEST : {}".format(experiment_name))
            assert (experiment_name != "")
            if self.rebroadcast_combo.currentText() == "yes" :
                rebroadcast = "1"
                                                #if not ebough information is provided, ends the test and print an error message
        except :                                                                                                #------------------PRINT------------------                
            print "INVALID INPUT cast : {}, burst_number : {}, interval : {}, rebroadcast : {}".format(cast_number , burst_number, interval , rebroadcast)
            return 
            
        node_number = len(node_list)
        if node_number == 0 :
            print "Insufficent number of node connected for the experiment !"                                    #------------------PRINT------------------    
            return
            
        if experiment_name != "tables" :        #One cannot name his experiment tables. It would mess up the database organization... and wouldn't make much sens.
            database_init(experiment_name,cast_number,burst_number, interval, rebroadcast, node_number)    
            #print ("Database init !")                                                                            #------------------PRINT------------------    
            self.test_thread.launching(experiment_name , burst_number , cast_number , interval , rebroadcast , node_number)
        
        else :
            output.append("'tables' is a reserved name, use another one if you want to conduct an experiment.\n")        #------------------PRINT------------------    

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
