#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Error: Pass in a run number and number of iterations for register test"
    exit 1
else 
    runNum=$1
    n=$2
    echo "The run number is:                          $1"
    echo "Number of iterations for register test is:  $n"
fi

REMOTEHOST=hep@cmshcal11

##################################################
# Run the register test
##################################################
echo "Running the register test"
cd /home/hcalpro/GITrepos/Common
python RunRegisterTest.py $n 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 &
#python RunRegisterTest.py $n 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 &

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
./processPlugins -f /home/hcalpro/hcalraw/output/run$runNum-master.root

##################################################
# Moves all data to long term storage on cmshcal11
##################################################
wait
./MoveFiles.sh

##################################################
# Upload cards to the data base
##################################################
ssh $REMOTEHOST '/home/django/testing_database_hb/uploader/upload_step2.sh'
