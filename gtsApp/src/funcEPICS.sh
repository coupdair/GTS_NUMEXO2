#!/bin/bash

echo "add to \"./dbSubExample.dbd\""
for i in `cat funcEPICS.txt`
do
  s=`echo $i | sed 's/(/ /;s/)//'`
  f=`echo $s | cut -d' ' -f1`
#  echo $i': '$f $a
  echo "function("$f"EPICS)"
done


echo "add to \"../Db/dbSubExample.db\""
for i in `cat funcEPICS.txt`
do
  s=`echo $i | sed 's/(/ /;s/)//'`
  f=`echo $s | cut -d' ' -f1`
  a=`echo $s | cut -d' ' -f2`
  echo 'record(sub,"$(user):'$f'")'
  echo '{'
  echo '    field(SCAN,"Passive")'
  echo '    field(SNAM,"'$f'EPICS")'
  echo '    field(DESC,"'$i'")'
  echo '}'
done

