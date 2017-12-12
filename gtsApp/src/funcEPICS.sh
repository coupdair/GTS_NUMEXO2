#!/bin/bash

echo "add to \"./dbSubExample.dbd\""
for i in `cat funcEPICS.txt`
do
  echo "function("$i"EPICS)"
done

echo "add to \"../Db/dbSubExample.db\""
for i in `cat funcEPICS.txt`
do
  echo 'record(sub,"$(user):'$i'")'
  echo '{'
  echo '    field(SCAN,"Passive")'
  echo '    field(SNAM,"'$i'EPICS")'
  echo '}'
done

