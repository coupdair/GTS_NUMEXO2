#!/bin/bash

have_nt=true
have_nt=false
#echo $have_nt

#get version (on host computer, else on target call "SC_server_numexo -v")
strings Linux-ppc440/gtsServer | grep 'v.\..\..' | head -n 1 > VERSION
###or
if($have_nt)
then
  nt42 /usr/local/bin/gtsServer -v | tail -c 8 > VERSION
fi

#command line help
##old UDP server
make -f Makefile.host
cd ./Linux-x86-64/
 ./gtsServer -v
 ./gtsServer -h > ../gtsServer.help.output
cd ..
##GTS server with EPICS+UDP
./bin/linux-x86_64/gts --help    > gts.help.output
./bin/linux-x86_64/gts --version > VERSION

#documentation
##version (tcsh)
VERSION=`cat VERSION`
cat Doxyfile.template | sed "s/##VERSION##/$VERSION/" > Doxyfile
##commands
###UDP
grep 'int.cmd_' udpInterpret.c | head -n 35 | sed 's/int.cmd_//;s/.(void\*)\;//' > gtsServer_command_UDP.txt
###EPICS
cat gtsApp/Db/dbSubExample.db | grep DESC | cut -d',' -f2- | sed 's/"//g;s/)//' | grep -v __SERVER_VERSION__ > gtsServer_command_EPICS.txt
###UDP<->EPICS table
for w in `cat gtsServer_command_UDP.txt`
do
  w2=`grep $w'(' gtsServer_command_EPICS.txt`
  printf "%-32s%s\n" $w $w2
done > gtsServer_command.txt

##make (html and prepare latex)
doxygen
ls doc/html/index.html
firefox doc/html/index.html &

exit

##make latex
cd doc/latex
make

