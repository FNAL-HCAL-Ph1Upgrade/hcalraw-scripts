#!/bin/bash

##################################################
# Moves all data to long term storage on cmshcal11
##################################################
runNum=$1
moveReg=$2
moveQC=$3
REMOTEHOST=hep@cmshcal11
UploadDir=/home/django/testing_database_hb/media/uploads
QCPath=/home/hcalpro/hcalraw-scripts/processPluginsHBQIE/qcTestResults

#Upload file that are already in cards/temp_reg_test: 1 : just QC, 2 : just reg., 3 : both
#ssh $REMOTEHOST '/home/django/testing_database_hb/uploader/upload_step2.sh 3'

echo "Moving files to cmshcal11"
##################################################
# Move Register test results
##################################################
if [ $moveReg -eq 1 ]; then
    rsync -r /home/hcalpro/GITrepos/Common/registerTestResults/* $REMOTEHOST:$UploadDir/temp_reg_test/.
    rsync -r /home/hcalpro/GITrepos/Common/registerTestResults/* /home/hcalpro/DATA/RegTestResults/.
    rm -rf /home/hcalpro/GITrepos/Common/registerTestResults/0x*
fi

##################################################
# Move QC Scan results
##################################################
if [ $moveQC -eq 1 ]; then
    echo "Making directory on cmshcal11"
    ssh $REMOTEHOST 'cd '$UploadDir'/run_control && mkdir -p run'${runNum}'_output'
    
    #Output of run control
    rsync  /home/hcalpro/DATA/FNAL_000$runNum.root $REMOTEHOST:$UploadDir/run_control/run${runNum}_output/.
    rsync  /home/hcalpro/DATA/FNAL_000$runNum.root /run/media/hcalpro/HCALWH14/RunControlOutput2018/.
    
    #Output of Mark's plugin
    rsync /home/hcalpro/hcalraw/output/run$runNum-master.root $REMOTEHOST:$UploadDir/run_control/run${runNum}_output/.

    #Output of Chris' QC code
    for dir in $QCPath/QC_run$runNum/*; do
	if [ -d "$dir" ]; then
    	    #Card info
    	    rsync -a $dir $REMOTEHOST:$UploadDir/run_control/cards/.
	else
    	    #Run info
    	    rsync $dir $REMOTEHOST:$UploadDir/run_control/run${runNum}_output/.
	fi
    done
fi
