#include <stdio.h>

#include <dbDefs.h>
#include <registryFunction.h>
#include <subRecord.h>
#include <aSubRecord.h>
#include <epicsExport.h>

int mySubDebug;

static int increment;

static long mySubInit(subRecord *precord)
{
    if (mySubDebug)
        printf("Record %s called mySubInit(%p)\n",
               precord->name, (void*) precord);
    return 0;
}

static long mySubProcess(subRecord *precord)
{
    if (mySubDebug)
        printf("Record %s called mySubProcess(%p)\n",
               precord->name, (void*) precord);
    return 0;
}

static long myAsubInit(aSubRecord *precord)
{
  increment=0;
  if (mySubDebug)
    printf("Record %s called myAsubInit(%p) #%d\n"
      , precord->name, (void*) precord
      , increment);
    return 0;
}

static long myAsubProcess(aSubRecord *precord)
{
  ++increment;
  if (mySubDebug)
    printf("Record %s called myAsubProcess(%p) #%d\n"
      , precord->name, (void*) precord
      , increment);
    return 0;
}

/* Register these symbols for use by IOC code: */

//debug
//e.g.: st.cmd: var mySubDebug 1
//or interactive: epics> var mySubDebug 0
epicsExportAddress(int, mySubDebug);

//user called
//e.g.: caput coudert:subExample 123.456
epicsRegisterFunction(mySubInit);
epicsRegisterFunction(mySubProcess);

//periodicaly called
epicsRegisterFunction(myAsubInit);
epicsRegisterFunction(myAsubProcess);

