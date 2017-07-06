#include <stdio.h>

#include <dbDefs.h>
#include <registryFunction.h>
#include <subRecord.h>
#include <aSubRecord.h>
#include <epicsExport.h>

//GTS device
#include "../../xmmregs_l.h" //CARD_NUMBER_ADDRESS
#include "../../xstatus.h"   //XST_SUCCESS
#include "../../xmmregs_i.h"//cardNumber

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
  precord->val = increment;
  return increment;
}

//! EPICS call for \c gtsReset function
unsigned long mygtsReset(subRecord *precord)
{
#ifndef _X86_64_
  int status = XST_SUCCESS;
  unsigned long  cardnumber = *((unsigned long *) CARD_NUMBER_ADDRESS);

  if((int)precord->val != 1) {
    return 0;
  }

  if (mySubDebug)
       printf("gts %lu is being reset\n",  cardnumber);
   status=gtsReset();
#else
  int status=0;
  if (mySubDebug)
       printf("gts fake: EPICS/%s(subRecord *)\n",__func__);
#endif //_X86_64_
  precord->val = status;
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

//GTS
epicsRegisterFunction(mygtsReset);

