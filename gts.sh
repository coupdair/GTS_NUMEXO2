#run GTS server either UDP or EPICS on NumExo2 module
##gts.v0.3.

killall gts
cd /usr/local/bin/

##UDP
#./gts -i $id &

##EPICS
export EPICS_CA_ADDR_LIST="10.10.17.254"
./caRepeater &
id=`cat /GTSid`
sed "s/__GTS_ID__/$id/" /usr/local/bin/envGTS_ID > /usr/local/bin/envGTSid
./gts --epics -c gts.cmd -i $id

