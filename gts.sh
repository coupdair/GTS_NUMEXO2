#!/bin/ash

#run GTS server either UDP or EPICS on NumExo2 module
##gts.v0.3.
# setup specific GTS id in "/GTSid" (e.g. echo 123 > /GTSid)
# run with EPICS, if /GTS_EPICS exists   (e.g. touch /GTS_EPICS)
# run with UDP, if /GTS_EPICS do NOT exists (e.g. rm /GTS_EPICS)

console=' > /dev/console < /dev/console 2> /dev/console 3> /dev/console'

killall gts caRepeater
cd /usr/local/bin/

#GTS id: from /GTSid, otherwize default (i.e. from last byte of IP address)
gtsOpt=
if [ -e /GTSid ]
then
  id=`cat /GTSid`
  gtsOpt='-i '$id
else
  ip=`/sbin/ifconfig eth0 | grep 'addr:' | cut -d: -f2 | cut -d' ' -f1`
  id=`echo $ip | cut -d. -f4`
fi

#service as UDP or EPICS, if /GTS_EPICS exist
if [ -e /GTS_EPICS ]
then
  ##EPICS
  export EPICS_CA_ADDR_LIST="10.10.17.254"
  ./caRepeater &
  #set EPICS env
  sed "s/__GTS_ID__/$id/" /usr/local/bin/envGTS_ID > /usr/local/bin/envGTSid
  ./gts --epics -c gts.cmd $gtsOpt $console &
else
  ##UDP
  ./gts $gtsOpt $console &
fi

