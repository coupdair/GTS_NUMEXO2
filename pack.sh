#!/bin/bash

#a few color in csh
TC_LightGreen="\033[1;32m"
TC_LightCyan="\033[1;36m"
TC_clear="\033[0m"

#example to set "envGTSid" file (will be reset by "gts.sh" script in embedded target)
id=123

#GTS id
echo 'set GTSid to '$id
sed "s/__GTS_ID__/$id/"  gtsApp/src/envGTS_ID > envGTSid
cp -p iocBoot/iocgts/st.cmd iocBoot/iocgts/gts.cmd

#pack
cp -p /space/global/buildroot/CentOS6_64/ppc440/epics/base-3.15.5/bin/linux-ppc/caRepeater bin/linux-ppc/
rm gts.zip ; zip -ry9 gts.zip db dbd ; zip -j gts.zip gts.sh bin/linux-ppc/gts envGTSid gtsApp/src/envGTS_ID iocBoot/iocgts/gts.cmd bin/linux-ppc/caRepeater
##should be unpacked in /usr/local/bin/ of numexo2, e.g. cd /usr/local/bin/; unzip -o /tmp/gts.zip

sync
#GUI notify
notify-send 'GTS packing ends' 'GTS server packing ends for gts'$id',\ni.e. '$0' from "'$PWD'".' --icon=/usr/share/hardinfo/pixmaps/compress.png -t 4321

