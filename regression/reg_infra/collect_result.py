#!/usr/bin/python

import os
import glob
import sys

path="/home/juexiao/Research/qpr/benchmark/first_task"
if len(sys.argv) != 4:
  print("Usage: collect_result.py <ref_log> <new_log> <result>")
  exit(1)

ref_name=sys.argv[1]
new_name=sys.argv[2]
result_file=sys.argv[3]
s=open(result_file,'w')

s.write(",,----,ref,----,,,")
s.write(",,----,new,----,,,")
s.write(",,----,Improvement,----\n")

s.write("Design,Gate#,Net#,Suc,Iter#,WireLength,RunTime,")
s.write(",Design,Gate#,Net#,Suc,Iter#,WireLength,RunTime,")
s.write(",WireLength,%,RunTime,%\n")


def get_state_single(filename, design_name):
  re=0
  f=open(filename)
  cod = design_name
  while 1:
    line =f.readline()
    if not line:
      break
    result='not found'
    if line.find('This SAT problem') != -1:
      temp = line;
      gate_num = temp.split(' ')[7]
      net_num = temp.split(' ')[9]
      cod = cod + ',' + gate_num + ','+net_num

    if line.find('elapsed time') != -1:
      temp = line;
      time = temp.split(' ')[2].rstrip("\n")
      time = "{0:.2f}".format(float(time))
      if len(cod.split(",")) == 1:
        cod = cod + ",,,,,"
      cod = cod+ ','+time
      #print cod
      return cod

    a=line.find('Successfully routed')
    if not a==-1:
      re=1
      result=line
      num1=result.split(' ')[3]
      num2=result.split(' ')[7]
      num3=num2.split('.')[0]
      cod=cod+','+'succeed'+','+num1+','+num3

    b=line.find('Routing failed')
    if not b==-1:
      re=1
      result=line
      cod=cod+','+'failed,,'
  f.close()


ave_wire_length_impr = 0.0
ave_runtime_impr = 0.0
count = 0
for filename in os.listdir(path):

  file_ref=os.path.join(path,filename,ref_name)
  file_new=os.path.join(path,filename,new_name)

  ref_result = get_state_single(file_ref, filename)
  new_result = get_state_single(file_new, filename)

  if ref_result is None:
    continue

  if len(ref_result.split(",")) < 5:
    continue

  ref_wirelength = ref_result.split(",")[5]
  new_wirelength = new_result.split(",")[5]

  ref_runtime = "{0:.2f}".format(float(ref_result.split(",")[6]))
  new_runtime = "{0:.2f}".format(float(new_result.split(",")[6]))

  impr_wirelength = "NA"
  impr_runtime = "NA"
  perc_wirelength = "NA"
  perc_runtime = "NA"
  if ref_result.split(",")[3] == "succeed" and new_result.split(",")[3] == "succeed":
    count = count + 1;
    impr_wirelength = str(int(ref_wirelength) - int(new_wirelength))
    impr_runtime = str(float(ref_runtime) - float(new_runtime))
    perc_wirelength = "{0:.2f}".format(float(impr_wirelength)/float(ref_wirelength)*100) + "%"
    perc_runtime = "{0:.2f}".format(float(impr_runtime)/float(ref_runtime)*100) + "%"
    ave_wire_length_impr = ave_wire_length_impr + float(impr_wirelength)/float(ref_wirelength)
    ave_runtime_impr = ave_runtime_impr + float(impr_runtime)/float(ref_runtime)

  s.write(ref_result + ",," + new_result + ",," + impr_wirelength + ',' + perc_wirelength + "," + impr_runtime + "," + perc_runtime + "\n")

ave_wire_length_impr = ave_wire_length_impr / count *100
ave_runtime_impr = ave_runtime_impr / count * 100
ave_wire_length_impr = "{0:.2f}".format(ave_wire_length_impr) + "%"
ave_runtime_impr = "{0:.2f}".format(ave_runtime_impr) + "%"


s.write("Summary\n")
s.write("Wirelength Improvement," + ave_wire_length_impr + "\n")
s.write("Runtime Improvement," + ave_runtime_impr + "\n")
s.close()
