import multiprocessing
import time
import re
import dewi as DBConnection

class StatsCollectionProcess(multiprocessing.Process):

    dewi = DBConnection.dewi()

    def __init__(self, output_queue, feedback_queue):
        multiprocessing.Process.__init__(self)
        self.output_queue = output_queue
        self.feedback_queue = feedback_queue
        
    def run(self):
        
        while True:
            nodes = self.dewi.getNodeslist()
            for node in nodes:
                nd = re.sub(':', '', node[0])
                i=0
                while (self.feedback_queue.empty() and i < 5):
                    self.output_queue.put("0x" + nd + "\n")
                    time.sleep(15) # wait for 15 seconds before next polling
                    i += 1
                if (not self.feedback_queue.empty()):
                    self.feedback_queue.get()
            self.output_queue.put("0x0000\n") # indicate to the network that this polling cycle is over
            time.sleep(120) # wait for 2 min before looping through the list of nodes again
                
                
            
            
