#!/usr/bin/python


#read *.blif
#strash
#refactor
#write *_abc.blif
#
#run to create at same time
import subprocess
import os
import glob
import time

root=os.getenv("QSAT_HOME")
path=os.path.join(root, 'regression/testcases')


for filename in os.listdir(path):
  nextfile=os.path.join(path,filename)

  #cnf
  name=filename
  filenameex=name+'_cnf2blif.tcl'
  filetxt=os.path.join(path,filename,filenameex)
  name1=name+'.cnf'
  readname=os.path.join(path,filename,name1)
  name2=name+'.blif'
  writename=os.path.join(path,filename,name2)
  #f=open(filetxt,'w')
  #f.write('cnf2blif')
  #f.write(' ')
  #f.write(name1)
  #f.write(' ')
  #f.write(name2)
  #f.write("\n")
  #f.write("exit\n")
  #f.close()
  cod="cd " + os.path.join(path,filename)+";"
  cod=cod+'qSat < '+ filetxt

  #abc tool
  abc_name = name + '_abc.tcl'
  abc_script = os.path.join(path, filename, abc_name)
  s=open(abc_script, 'w')
  name3=name+"_abc.blif"
  readname=writename
  writename=os.path.join(path,filename,name3)

  s.write('read_blif')
  s.write(' ')
  s.write(readname)
  s.write("\n")
  s.write('strash')
  s.write("\n")
  s.write('refactor')
  s.write("\n")
  s.write('write_blif')
  s.write(' ')
  s.write(writename)
  s.close()
  cod='abc -f ' + abc_script
  #print cod
  os.system(cod)

  #get_stat = name + '_get_stat.tcl'
  #get_stat_script = os.path.join(path, filename, get_stat)
  #outfile = open(get_stat_script, 'w')
  #outfile.write('readBlif ')
  #outfile.write(writename + '\n')
  #outfile.write("genDwaveNetlist\n")
  #outfile.write("exit\n")
  #outfile.close()
  


  #print cod
  #os.system(cod)
  #cnf_result = os.path.join(path, filename, "cnf_stat")
  #cod='dwave < ' + get_stat_script + ' >& ' + cnf_result
  #cod='/bin/bash -c "' + cod + '"'
  #print cod
  #os.system(cod)
