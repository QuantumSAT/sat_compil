#!/usr/bin/python

import glob
import os
import shutil

root = os.getenv("QSAT_HOME")

path = os.path.join(root, "regression/blifs/*.blif")
blif_path= os.path.join(root, "regression/testcases")
reg_infra = os.path.join(root, "regression/reg_infra")

for filename in glob.glob(path):   
    #print filename
    name_with=filename.split("/")[-1]
    name=filename.split("/")[-1].rstrip(".blif")
    if name.find("#") != -1:
      name = name.replace("#","-")
    if name.find(".") != -1:
      name = name.replace(".","-")
    path_to=os.path.join(blif_path,name)
    path_exist=os.path.exists(path_to)
    if path_exist:
      #print 'exist already, will move'
        path_to_to=os.path.join(path_to,name+".blif")
        #print path_to_to
        shutil.copy(filename,path_to_to)
        shutil.copy(reg_infra + "/template.tcl", path_to + "/design.tcl")
    else:
      #print 'not exist, will create'
        os.makedirs(path_to)
        path_to_to=os.path.join(path_to,name+".blif")
        #print path_to_to
        shutil.copy(filename,path_to_to)
        shutil.copy(reg_infra + "/template.tcl", path_to + "/design.tcl")
