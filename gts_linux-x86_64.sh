#!/bin/bash
#run GTS server either UDP or EPICS on NumExo2 module

killall gts
killall caRepeater
cd ~/GTS_server/GTS_NUMEXO2/

#check
pwd
cat envGTSid

##UDP
#./gts &

##EPICS
#todo: get IP and set it in gts.cmd
export EPICS_CA_ADDR_LIST="10.10.17.254"
~/bin/caRepeater &
./bin/linux-x86_64/gts --epics -c iocBoot/iocgts/gts.cmd

