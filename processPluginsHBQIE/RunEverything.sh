#!/bin/bash

if [ $# -lt 4 ]; then
    echo "Error: Pass in a run number, number of iterations for register test, tester's name, and comments"
    exit 1
else 
    runNum=$1
    n=$2
    name=$3
    comments=$4
    echo "The run number is:                          $1"
    echo "Number of iterations for register test is:  $n"
fi

REMOTEHOST=hep@cmshcal11

##################################################
# Run the register test
##################################################
echo "Running the register test"
cd /home/hcalpro/GITrepos/Common
python RunRegisterTest.py $n 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 "$name" "$comments" &
#python RunRegisterTest.py $n 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "$name" "$comments" &

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

#Move reg. test stuff
wait
./MoveFiles.sh $runNum 1 0
#1 : just QC, 2 : just reg., 3 : both
ssh $REMOTEHOST '/home/django/testing_database_hb/uploader/upload_step2.sh 2'
