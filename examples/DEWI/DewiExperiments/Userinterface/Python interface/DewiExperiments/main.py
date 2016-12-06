# -*- coding: utf-8 -*-


import sys
import time
import numpy
import serial
import thread
import sqlite3
import datetime
import threading
import windowClass
from PyQt4 import QtGui
from cycler import cycler
from PyQt4.QtCore import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar

def main():

	try:
		
		app = QtGui.QApplication(sys.argv)
		mainwindow = windowClass.MainWindow()
		
		#thread.start_new_thread (receiving,( ))
		#thread.start_new_thread (store,( ))
		#thread.start_new_thread (sys.exit,(app.exec_() , ))
	
	finally :
	
		print("Closing before leaving!")				#When interrutping the programm, the list has to be saved in a file

		

  
main()