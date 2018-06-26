#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Pass in a run number.....please :("
    exit 1
else 
    runNum=$1
fi

REMOTEHOST=hep@cmshcal11
UploadDir=/home/django/testing_database_hb/media/uploads
QCPath=/home/hcalpro/hcalraw-scripts/processPluginsHBQIE/qcTestResults

##################################################
# Run the register test
##################################################
#echo "Running the register test"
#cd /home/hcalpro/GITrepos/Common
#python RunRegisterTest.py 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 &
#python RunRegisterTest.py 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 &

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
#wait
#echo "Making directory on cmshcal11"
#ssh $REMOTEHOST 'cd '$UploadDir'/run_control && mkdir run'${runNum}'_output'
#
#echo "Moving files to cmshcal11"
##Output of Frank's register test
#rsync -r /home/hcalpro/GITrepos/Common/registerTestResults/* $REMOTEHOST:$UploadDir/temp_reg_test/.
#
##Output of run control
#rsync  /home/hcalpro/DATA/FNAL_000$runNum.root $REMOTEHOST:$UploadDir/run_control/run${runNum}_output/.
#rsync  /home/hcalpro/DATA/FNAL_000$runNum.root /run/media/hcalpro/HCALWH14/RunControlOutput2018/.
#
##Output of Mark's plugin
#rsync /home/hcalpro/hcalraw/output/run$runNum-master.root $REMOTEHOST:$UploadDir/run_control/run${runNum}_output/.
#
##Output of Chris' QC code
#for dir in $QCPath/QC_run$runNum/*; do
#    if [ -d "$dir" ]; then
#    	#Card info
#    	rsync -a $dir $REMOTEHOST:$UploadDir/run_control/cards/.
#    else
#    	#Run info
#    	rsync $dir $REMOTEHOST:$UploadDir/run_control/run${runNum}_output/.
#    fi
#done
