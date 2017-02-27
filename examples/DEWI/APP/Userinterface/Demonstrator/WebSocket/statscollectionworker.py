import multiprocessing
import time
import re
import dewi as DBConnection

class StatsCollectionProcess(multiprocessing.Process):

    dewi = DBConnection.dewi()

    def __init__(self, output_queue):
        multiprocessing.Process.__init__(self)
        self.output_queue = output_queue
        
    def run(self):
        
        while True:
            nodes = self.dewi.getNodeslist()
            for node in nodes:
                nd = re.sub(':', '', node[0])
                self.output_queue.put("0x" + nd + "\n")
                time.sleep(15) # wait for 15 seconds before polling next node
            time.sleep(300) # wait for 5 min before looping through the list of nodes again
                
                
            
            