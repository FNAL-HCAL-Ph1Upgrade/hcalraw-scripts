#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Error: Pass in a run number list, tester's name, and comments"
    exit 1
else 
    runNum=$1
    name=$2
    comments=$3
    #echo "The run number is:                          $1"
    #echo "Number of iterations for register test is:  $n"
fi

REMOTEHOST=hep@cmshcal11

##################################################
# Processes the output of run control
##################################################
echo "Process run control"
cd /home/hcalpro/hcalraw
./fnal_analyze.sh $runNum master

##################################################
# Processes the output of hcalraw
##################################################
echo "Process run hcalraw"
cd /home/hcalpro/hcalraw-scripts/processPluginsHBQIE
# Setup python3 root6 environment
source /opt/root6_py3/bin/thisroot.sh
./processPlugins -f /home/hcalpro/hcalraw/output/run$runNum-master.root -n "$name" -c "$comments"

#########################################################################
# Moves all data to long term storage on cmshcal11 and upload to database
#########################################################################

#Move run control stuff
./MoveFiles.sh $runNum 0 1
#1 : just QC, 2 : just reg., 3 : both
ssh $REMOTEHOST '/home/django/testing_database_hb/uploader/upload_step2.sh 1'
