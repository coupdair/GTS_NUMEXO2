#hand compile

#a few color in csh
set TC_LightGreen="\033[1;32m"
set TC_LightCyan="\033[1;36m"
set TC_clear="\033[0m"

#clean
rm -f bin/linux-*/* Linux-*/* \
  gtsApp/src/O.linux-ppc/*.o gtsApp/src/O.linux-ppc/*.d gtsApp/src/O.linux-ppc/*.a
make clean
#rm -fr iocBoot db/* dbd include lib
#sync
##rebuild IOC
#/space/global/buildroot/CentOS6_64/ppc440/epics/base/bin/linux-x86_64/makeBaseApp.pl -t example -i -p gts -a linux-x86_64 gts
sync
#option: copy caRepeater
cp -p /space/global/buildroot/CentOS6_64/ppc440/epics/base-3.15.5/bin/linux-ppc/caRepeater bin/linux-ppc/

##compile device and UDP objects
make -f Makefile.target
##compile EPICS objects (error on PPC, due to missing objects in linkage)
make
/bin/echo -e ${TC_LightCyan}"information:"${TC_clear}" error on PPC, due to missing objects in linkage"

##link by hand
cd /home/coudert/GTS_server/GTS_NUMEXO2/gtsApp/src/O.linux-ppc/
rm ../../../Linux-ppc440/gtsServer.o
set list=`ls ../../../Linux-ppc440/*.o`
/bin/echo -e ${TC_LightCyan}"linking: "${TC_LightGreen}$list${TC_clear}
##link all device, EPICS and UDP (e.g. with gtsMisc.o)
/space/global/buildroot/CentOS6_64/ppc440/buildroot-2013.11/output/host/usr//bin/powerpc-buildroot-linux-gnu-g++ -o gts -Wl,-Bstatic -L/home/coudert/GTS_server/GTS_NUMEXO2/lib/linux-ppc -L/space/global/buildroot/CentOS6_64/ppc440/epics/base-3.15.5/lib/linux-ppc -Wl,-rpath,/home/coudert/GTS_server/GTS_NUMEXO2/lib/linux-ppc -Wl,-rpath,/space/global/buildroot/CentOS6_64/ppc440/epics/base-3.15.5/lib/linux-ppc -rdynamic -L/space/global/buildroot/CentOS6_64/ppc440/buildroot-2013.11/output/host/usr//powerpc-buildroot-linux-gnu/lib gts_registerRecordDeviceDriver.o gtsMain.o -lgtsSupport -ldbRecStd -ldbCore -lca -lCom -Wl,-Bdynamic -lpthread -lm -lrt -ldl -lgcc \
  $list
/bin/echo -e ${TC_LightCyan}"information:"${TC_clear}" linkage EPICS with device+UDP"
ls -lah ./gts
file ./gts
###sync
##copy device+EPICS+UDP binary
cp -p gts ../../../bin/linux-ppc/
cd ../../../
ls -lah ./bin/linux-ppc/gts
file ./bin/linux-ppc/gts
date

#VERSION
set version=`./bin/linux-x86_64/gts --version`
set ver=`echo $version | sed 's/v//'`
set major=`echo $ver | cut -d'.' -f1`
set minor=`echo $ver | cut -d'.' -f2`
set rev=`echo $ver   | cut -d'.' -f3 | tr -d \[a-z\]`
set dev=`echo $ver   | tr -d \[.,0-9\]`
set dev=`printf %d 0x$dev`
set mNm=$major.$minor
cat gtsApp/Db/dbSubExample.db | sed "s/__SERVER_VERSION__/$version/;s/__SERVER_VERSION_MAJOR__/$major/;s/__SERVER_VERSION_MINOR__/$minor/;s/__SERVER_VERSION_REVISION__/$rev/;s/__SERVER_VERSION_DEVELOPMENT__/$dev/;s/__SERVER_VERSION_MAJORnMINOR__/$mNm/" > db/dbSubExample.db
#grep serverVersion -A 7 db/dbSubExample.db
##print
grep serverVersion -A 2 db/dbSubExample.db | grep DESC | grep 'v.\..\..' --color
sync

#GUI notify
notify-send 'GTS compilation ends' 'GTS server '$version' compilation ends, i.e. '$0' from "'$PWD'".' --icon=/usr/share/hardinfo/pixmaps/os.png -t 4321

