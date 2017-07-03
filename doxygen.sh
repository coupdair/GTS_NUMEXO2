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
make -f Makefile.host
cd ./Linux-x86-64/
 ./gtsServer -v
 ./gtsServer -h > ../gtsServer.help.output
cd ..

#documentation
##version (tcsh)
VERSION=`cat VERSION`
cat Doxyfile.template | sed "s/##VERSION##/$VERSION/" > Doxyfile
##help (need updated numexo2- conn ection)
nt12 /usr/local/bin/gtsServer -h > gtsServer.help.output
###or
./Linux-x86-64/gtsServer -h > gtsServer.help.output
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
