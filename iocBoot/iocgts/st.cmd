#!../../bin/linux-x86_64/gts

## You may have to change gts to something else
## everywhere it appears in this file

< envGTSid

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/gts.dbd"
gts_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords "db/dbSubExample.db", "user=${GTSID}"

## Set this to see messages from mySub
var mySubDebug 1

## Run this to trace the stages of iocInit
#traceIocInit

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs

##IOC shell examples:
#dbl
#dbpr gts127:serverVersion

