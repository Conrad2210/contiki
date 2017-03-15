import _mysql as mysql

class  dewi():

	dbConnection = 0
	db = 'DEWI_database';
	tab1 = 'Dewi_tab';
	tab2 = 'Dewi_linkTab';
	tab3 = 'Dewi_statsTab';
	items = 'addr,  a_target, insert_order';				 
	def __init__(self):
		self.dbConnection = mysql.connect(host="localhost",unix_socket='/var/run/mysqld/mysqld.sock',user="DEWI", passwd="dewiPass",db='DEWI_database')
	
	def DBconnection_(self):		
		try:
			self.dbConnection.query("SELECT VERSION()")
			result = self.dbConnection.use_result()
			print "MySQL version: %s" % result.fetch_row()[0]
        
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def handle_tab(self,addrS):
		self.dbConnection.query("use "+self.db+';');
		self.save_node_details(addrS);
	
	def  save_node_details(self,addrS,addrM,order):
		connection = self.dbConnection
		tab1 = self.tab1
		try:
                        if (addrM=="0:0"):
				connection.query("INSERT INTO " + str(tab1) + " (addr,a_target,insert_order) SELECT * FROM (SELECT '"+str(addrS)+"', '"+str(addrM)+"', "+str(order)+") AS tmp WHERE NOT EXISTS (SELECT * FROM "+str(tab1)+" WHERE addr = '"+str(addrS)+"');")
			else:
	                        connection.query("INSERT INTO " + str(tab1) + " (addr,a_target,insert_order) VALUES ('"+str(addrS)+"', '"+str(addrM)+"', "+str(order)+") ON DUPLICATE KEY UPDATE a_target='"+str(addrM)+"';")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def add_temperature_data(self,Taddr, temp):
		connection = self.dbConnection
		try:
			connection.query("UPDATE Dewi_tab SET temperature = '"+str(temp)+"' WHERE EXISTS (SELECT addr FROM (select '"+str(Taddr)+"') AS tmp WHERE addr = '"+str(Taddr)+"');")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def add_battery_stat(self,Baddr, Batt):
		connection = self.dbConnection
		try:
			connection.query("UPDATE Dewi_tab SET battery = '"+str(Batt)+"' WHERE EXISTS (SELECT addr FROM (select '"+str(Baddr)+"') AS tmp WHERE addr = '"+str(Baddr)+"');")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)

	def add_colour_stat(self,Caddr, Colour):
		connection = self.dbConnection
		try:
			connection.query("UPDATE Dewi_tab SET colour = '"+str(Colour)+"' WHERE EXISTS (SELECT addr FROM (select '"+str(Caddr)+"') AS tmp WHERE addr = '"+str(Caddr)+"');")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def add_statistics(self,addr, packets, latency):
		connection = self.dbConnection
		latency = latency*10;
		try:
			connection.query("INSERT INTO Dewi_statsTab (addr, packets, latency) VALUES ('"+str(addr)+"',"+str(packets)+","+str(latency)+") ON DUPLICATE KEY UPDATE packets = packets +"+str(packets)+", latency = "+str(latency)+";")
			result = connection.use_result()
			#print "INSERT INTO Dewi_statsTab (addr, packets, latency) VALUES ('"+str(addr)+"',"+str(packets)+","+str(latency)+") ON DUPLICATE KEY UPDATE packets = "+str(packets)+", latency = "+str(latency)
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def cleanDB(self):
		connection = self.dbConnection
		tab1 = self.tab1
		tab2 = self.tab2
		tab3 = self.tab3
		try:
			connection.query("TRUNCATE "+str(tab1)+";")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			connection.query("TRUNCATE "+str(tab2)+";")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			connection.query("TRUNCATE "+str(tab3))
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def cleanStatsTable(self):
		connection = self.dbConnection
		tab3 = self.tab3
		try:
			connection.query("TRUNCATE "+str(tab3))
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def cleanNodeLinkTables(self):
		connection = self.dbConnection
		tab1 = self.tab1
		tab2 = self.tab2
		try:
			connection.query("TRUNCATE "+str(tab1)+";")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
		try:
			connection.query("TRUNCATE "+str(tab2)+";")
			result = connection.use_result()
			print "MySQL version: %s" % result
		except mysql.Error, e:
			try:
				print "MySQL Error [%d]: %s" % (e.args[0], e.args[1])
			except IndexError:
				print "MySQL Error: %s" % str(e)
				
	def getNodeslist(self):
		connection = self.dbConnection
		try:
			connection.query("SELECT addr from Dewi_tab;")
			result = connection.store_result()
			return result.fetch_row(0)
		except mysql.Error, e:
			print "MySQL Error: %s" % str(e)
