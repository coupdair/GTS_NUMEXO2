#!/bin/bash

#a few color in csh
TC_LightGreen="\033[1;32m"
TC_LightCyan="\033[1;36m"
TC_clear="\033[0m"

id=127
id=`cat GTS_ID`

#GTS id
id=gts$id
echo 'set GTSid to '$id
sed "s/numexo2/$id/"    iocBoot/iocgts/st.cmd >  iocBoot/iocgts/gts.cmd

#pack
cp -p /space/global/buildroot/CentOS6_64/ppc440/epics/base-3.15.5/bin/linux-ppc/caRepeater bin/linux-ppc/
rm gts.zip ; zip -ry9 gts.zip db dbd ; zip -j gts.zip gts.sh bin/linux-ppc/gts iocBoot/iocgts/gts.cmd iocBoot/iocgts/st.cmd bin/linux-ppc/caRepeater
##should be unpacked in /usr/local/bin/ of numexo2, e.g. cd /usr/local/bin/; unzip -o /tmp/gts.zip

