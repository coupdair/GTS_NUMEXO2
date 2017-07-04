#hand compile

#clean
rm -f bin/linux-*/* Linux-*/*
###sync

##compile device and UDP objects
make -f Makefile.target
##compile EPICS objects (error on PPC, due to missing objects in linkage)
make
echo "information: error on PPC, due to missing objects in linkage"

##link by hand
cd /home/coudert/GTS_server/GTS_NUMEXO2/gtsApp/src/O.linux-ppc/
rm ../../../Linux-ppc440/gtsServer.o
##link all device, EPICS and UDP (e.g. with gtsMisc.o)
/space/global/buildroot/CentOS6_64/ppc440/buildroot-2013.11/output/host/usr//bin/powerpc-buildroot-linux-gnu-g++ -o gts -Wl,-Bstatic -L/home/coudert/GTS_server/GTS_NUMEXO2/lib/linux-ppc -L/space/global/buildroot/CentOS6_64/ppc440/epics/base-3.15.5/lib/linux-ppc -Wl,-rpath,/home/coudert/GTS_server/GTS_NUMEXO2/lib/linux-ppc -Wl,-rpath,/space/global/buildroot/CentOS6_64/ppc440/epics/base-3.15.5/lib/linux-ppc -rdynamic -L/space/global/buildroot/CentOS6_64/ppc440/buildroot-2013.11/output/host/usr//powerpc-buildroot-linux-gnu/lib gts_registerRecordDeviceDriver.o gtsMain.o -lgtsSupport -ldbRecStd -ldbCore -lca -lCom -Wl,-Bdynamic -lpthread -lm -lrt -ldl -lgcc \
  `ls ../../../Linux-ppc440/*.o`
echo "information: linkage EPICS with device+UDP"
ls -lah ./gts
file ./gts
###sync
##copy device+EPICS+UDP binary
cp -p gts ../../../bin/linux-ppc/
cd ../../../
ls -lah ./bin/linux-ppc/gts
file ./bin/linux-ppc/gts

