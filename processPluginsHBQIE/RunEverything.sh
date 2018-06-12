#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Pass in a run number.....please :("
    exit 1
else 
    runNum=$1
fi

REMOTEHOST=hep@cmshcal11
UploadDir=/home/django/testing_database_hb/media/uploads/run_control

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
echo "Making directory on cmshcal11"
ssh $REMOTEHOST 'cd '$UploadDir'/ && mkdir run'${runNum}'_output'

echo "Moving files to cmshcal11"
#Output of run control
rsync  /tmp/FNAL_000$runNum.root $REMOTEHOST:$UploadDir/run${runNum}_output/.

#Output of Marks' plugis
rsync /home/hcalpro/hcalraw/output/run$runNum-master.root $REMOTEHOST:$UploadDir/run${runNum}_output/.

#Output of Chris' QC code 
rsync -r /home/hcalpro/hcalraw-scripts/processPluginsHBQIE/QC_run$runNum $REMOTEHOST:$UploadDir/run${runNum}_output/.
