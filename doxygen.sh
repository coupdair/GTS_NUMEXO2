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
grep 'int.cmd_' udpInterpret.c | head -n 35 | sed 's/int.cmd_//;s/.(void\*)\;//' > gtsServer_command.txt
##make (html and prepare latex)
doxygen
ls doc/html/index.html
firefox doc/html/index.html &

exit

##make latex
cd doc/latex
make

