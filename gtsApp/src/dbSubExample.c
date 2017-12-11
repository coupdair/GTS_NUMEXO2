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
static int subi;

static long mySubInit(subRecord *precord)
{
  subi=0;
  if (mySubDebug)
    printf("Record %s called mySubInit(%p/%f) #%d S%d\n"
      ,precord->name
      ,(void*) precord
      ,precord->val
      ,increment
      ,subi
    );
  return 0;
}//mySubInit
static long mySubProcess(subRecord *precord)
{
  ++subi;
  if (mySubDebug)
    printf("Record %s called mySubProcess(%p/%f) #%d S%d\n"
      ,precord->name
      ,(void*) precord
      ,precord->val
      ,increment
      ,subi
    );
  precord->val = increment;
  return 0;
}//mySubProcess

static long myAsubInit(aSubRecord *precord)
{
  if (mySubDebug)
    printf("Record %s called myAsubInit(%p) #%d\n"
      , precord->name, (void*) precord
      , increment);
  return 0;
}//myAsubInit
static long myAsubProcess(aSubRecord *precord)
{
  if (mySubDebug)
    printf("Record %s called myAsubProcess(%p) #%d\n"
      , precord->name, (void*) precord
      , increment);
  precord->val = increment;
  return increment;
}//myAsubProcess

static long gtsInit(subRecord *precord)
{
  increment=0;
  if(mySubDebug)
    printf("Record %s called gtsInit(%p/%f) #%d S%d\n"
      ,precord->name
      ,(void*) precord
      ,precord->val
      ,increment
      ,subi
    );
  return 0;
}//gtsInit
static long gtsProcess(subRecord *precord)
{
  ++increment;
  if(mySubDebug)
    printf("Record %s called gtsProcess(%p/%f) #%d S%d\n"
      ,precord->name
      ,(void*) precord
      ,precord->val
      ,increment
      ,subi
    );

#ifndef _X86_64_
  int status = XST_SUCCESS;
  unsigned long  cardnumber = *((unsigned long *) CARD_NUMBER_ADDRESS);

  if(precord->val != 1.0) return 0;

  if (mySubDebug)
       printf("gts %lu is being reset\n",  cardnumber);
  status=gtsReset();
#else
  int status=0;
  if(mySubDebug)
    printf("gts fake: EPICS/%s(subRecord *)\n",__func__);
#endif //_X86_64_
  precord->val = status;
  return 0;
}//gtsProcess

/* Register these symbols for use by IOC code: */
epicsExportAddress(int, mySubDebug);
epicsRegisterFunction(mySubInit);
epicsRegisterFunction(mySubProcess);
epicsRegisterFunction(gtsInit);
epicsRegisterFunction(gtsProcess);
epicsRegisterFunction(myAsubInit);
epicsRegisterFunction(myAsubProcess);

