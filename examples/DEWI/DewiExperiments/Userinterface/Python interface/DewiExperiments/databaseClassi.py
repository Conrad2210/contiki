import mysql.connector as mysql
import time


class  databaseConnection():

    dbConnection = 0
    db = 'dewi_experiments';                 
    def __init__(self):
        try:
            self.dbConnection = mysql.connect(host='localhost',user='root',db='dewi_experiments')
        except mysql.Error, e:
            self.dbConnection = mysql.connect(host='localhost',user='root')
            cursor = self.dbConnection.cursor()
            cursor.execute("CREATE DATABASE dewi_experiments DEFAULT CHARACTER SET 'utf8'")     
            self.dbConnection.database = 'dewi_experiments'
        try:            
            cursor = self.dbConnection.cursor()
            cursor.execute("SELECT * FROM experiments")
            for id in cursor:
                id
        except mysql.Error, e:
            try:
                self.dbConnection.cursor().execute("CREATE TABLE experiments (id INT NOT NULL AUTO_INCREMENT, description CHAR NULL, date_time INT(11) NULL, PRIMARY KEY(id))")
            except IndexError:
                print "MySQL Error: %s" % str(e)
                
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute("SELECT * FROM settings")
            for id in cursor:
                id
        
        except mysql.Error, e:
            try:
                self.dbConnection.cursor().execute("CREATE TABLE settings (id INT NOT NULL, session_id INT NOT NULL, txPower TEXT NOT NULL, numberBursts INT NOT NULL, burstDuration INT NOT NULL, MSGPerBurst INT NOT NULL, PRIMARY KEY(id), FOREIGN KEY(session_id) REFERENCES dewi_experiments.experiments(id))")
            except IndexError:
                print "MySQL Error: %s" % str(e)
                
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute("SELECT * FROM latency")
            for id in cursor:
                id
        
        except mysql.Error, e:
            try:
                self.dbConnection.cursor().execute("CREATE TABLE latency (id INT NOT NULL AUTO_INCREMENT, session_id INT NOT NULL,nodeID INT NOT NULL,timeslot INT NOT NULL, count INT NOT NULL, PRIMARY KEY(id), FOREIGN KEY(session_id) REFERENCES dewi_experiments.experiments(id))")
            except IndexError:
                print "MySQL Error: %s" % str(e)
                
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute("SELECT * FROM rxpackets")
            for id in cursor:
                id
        
        except mysql.Error, e:
            try:
                self.dbConnection.cursor().execute("CREATE TABLE rxpackets (id INT NOT NULL AUTO_INCREMENT, session_id INT NOT NULL,nodeID INT NOT NULL,count INT NOT NULL, PRIMARY KEY(id), FOREIGN KEY(session_id) REFERENCES dewi_experiments.experiments(id))")
            except IndexError:
                print "MySQL Error: %s" % str(e)
                
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute("SELECT * FROM txpackets")
            for id in cursor:
                id
        
        except mysql.Error, e:
            try:
                self.dbConnection.cursor().execute("CREATE TABLE txpackets (id INT NOT NULL AUTO_INCREMENT, session_id INT NOT NULL,nodeID INT NOT NULL,count INT NOT NULL, PRIMARY KEY(id), FOREIGN KEY(session_id) REFERENCES dewi_experiments.experiments(id))")
            except IndexError:
                print "MySQL Error: %s" % str(e)
  
    
    def insertExperiment(self,id,description,date_time):        
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute(("INSERT INTO experiments (id,description,date_time) VALUES ({0},'{1}',{2})").format(id,description,date_time))
            self.dbConnection.commit()
        except mysql.Error, e:
            try:
                print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
            except IndexError:
                print "MySQL Error: %s" % str(e)
  
    
    def insertSettings(self,id, session_id, txPower, numberBursts, burstDuration,MSGPerBurst):        
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute(("INSERT INTO settings(id, session_id, txPower, numberBursts, burstDuration,MSGPerBurst) VALUES ({0},{1},{2},{3},{4},{5})").format(id, session_id, txPower, numberBursts, burstDuration,MSGPerBurst))
            self.dbConnection.commit()
        except mysql.Error, e:
            try:
                print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
            except IndexError:
                print "MySQL Error: %s" % str(e)
                
    def insertLatency(self,id, session_id, nodeID,timeslot,count):        
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute(("INSERT INTO latency(id, session_id, nodeID,timeslot,count) VALUES ({0},{1},{2},{3},{4})").format(id, session_id, nodeID,timeslot,count))
            self.dbConnection.commit()
        except mysql.Error, e:
            try:
                print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
            except IndexError:
                print "MySQL Error: %s" % str(e)
                
    def insertRxPackets(self,id, session_id, nodeID,count):        
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute(("INSERT INTO rxpackets(id, session_id, nodeID,count) VALUES ({0},{1},{2},{3})").format(id, session_id, nodeID,count))
            self.dbConnection.commit()
        except mysql.Error, e:
            try:
                print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
            except IndexError:
                print "MySQL Error: %s" % str(e)
                
    def insertTxPackets(self,id, session_id, nodeID,count):        
        try:
            cursor = self.dbConnection.cursor()
            cursor.execute(("INSERT INTO txpackets(id, session_id, nodeID,count) VALUES ({0},{1},{2},{3})").format(id, session_id, nodeID,count))
            self.dbConnection.commit()
        except mysql.Error, e:
            try:
                print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
            except IndexError:
                print "MySQL Error: %s" % str(e)
                

    
    


