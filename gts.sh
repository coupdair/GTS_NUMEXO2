#run GTS server either UDP or EPICS on NumExo2 module

killall gts
cd /usr/local/bin/

##UDP
#./gts &

##EPICS
#todo: get IP and set it in gts.cmd
export EPICS_CA_ADDR_LIST="10.10.17.254"
./caRepeater &
./gts --epics -c gts.cmd


