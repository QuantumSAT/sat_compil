#!/usr/bin/python

import os
import glob
import sys
import time
import threading

root = os.getenv("QSAT_HOME")
path=os.path.join(root,"regression/testcases")
targets=[]
lock=False  

class testOneBlifSingleThread(threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self);

  def run(self):
    global lock
    global log_file
    while (not len(targets)==0):
        if not lock:
            lock=True
            temp=targets.pop()
            lock=False 
            start_time = time.time()
            #command = '/bin/bash -c ' + '"cd '+os.path.join(path,temp)+';'+'dwave < test_temp.tcl >& ' + log_file + '"'
            #print command
            os.system('/bin/bash -c ' + '"cd '+os.path.join(path,temp)+';'+'qSat design.tcl &> ' + log_file + '"')
            elapsed_time = time.time() - start_time
            print(temp + " elapsed " + str(elapsed_time))
            command = 'cd '+os.path.join(path,temp)+';' + 'echo "elapsed time: ' + str(elapsed_time) + '" >> '  + log_file
            #print command
            os.system(command)


if len(sys.argv) !=3:
  print"Usage: run_test.py <log file> <thread num>"
  exit(1)

log_file = sys.argv[1]
n_thread = int(sys.argv[2])
        
for filename in os.listdir(path):
    targets.append(filename)
    nextfile=os.path.join(path,filename)
    name=filename
    filenameex='design_name.tcl'
    filetxt=os.path.join(path,filename,filenameex)
    f=open(filetxt,'w')
    name1=name+'_abc.blif'
    f.write("set design " + name1)
    f.close()

print targets
threads = []
for i in range(0, n_thread):
    thread = testOneBlifSingleThread()
    thread.start()
    threads.append(thread)
  
for thread in threads:
    thread.join()

