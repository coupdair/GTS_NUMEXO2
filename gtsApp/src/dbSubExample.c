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

static long myGTSInit(subRecord *precord)
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
}//myGTSInit
static long myGTSProcess(subRecord *precord)
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
}//myGTSProcess

/* Register these symbols for use by IOC code: */
epicsExportAddress(int, mySubDebug);
epicsRegisterFunction(mySubInit);
epicsRegisterFunction(mySubProcess);
epicsRegisterFunction(myAsubInit);
epicsRegisterFunction(myAsubProcess);
epicsRegisterFunction(myGTSInit);
epicsRegisterFunction(myGTSProcess);

#define SUB_DEBUG_PRINT_COMMENT(COMMENT) \
  if(mySubDebug) \
    printf("Record %s called %s(%p/%f)" #COMMENT "\n" \
      ,precord->name \
      ,__func__ \
      ,(void*) precord \
      ,precord->val \
    );

#define SUB_DEBUG_PRINT SUB_DEBUG_PRINT_COMMENT(activated_if_1)

#ifndef _X86_64_

#define funcNoArgEPICS(ARG,FCT) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status = XST_SUCCESS; \
  unsigned long  cardnumber = *((unsigned long *) CARD_NUMBER_ADDRESS); \
 \
  if(precord->val != 1.0) return 0; \
 \
  if (mySubDebug) \
       printf("gts %lu is running " #FCT "\n",  cardnumber); \
  status=FCT(); \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcNoArgEPICS

#else //other ARCH

#define funcNoArgEPICS(ARG,FCT) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status=0; \
  if(mySubDebug) \
    printf("gts fake: EPICS/%s(subRecord *)\n",__func__); \
 \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcNoArgEPICS

#endif //_X86_64_

//funcNoArgEPICS(,)
funcNoArgEPICS(gtsReset,  gtsReset)
funcNoArgEPICS(gtsInit,   gtsInit)
funcNoArgEPICS(gtsTest,   gtsTest)
funcNoArgEPICS(gtsUpdate, gtsUpdate)
funcNoArgEPICS(conIs,  conIs)
funcNoArgEPICS(gtsIs,  gtsIs)
funcNoArgEPICS(treeStart, treeStart)
funcNoArgEPICS(treeStop,  treeStop)
funcNoArgEPICS(includeCarrierForAlign, includeCarrierForAlign)
funcNoArgEPICS(excludeCarrierForAlign, excludeCarrierForAlign)
funcNoArgEPICS(alignStart, alignStart)
funcNoArgEPICS(testSet, leaveTestLoopback)
funcNoArgEPICS(triggerCoreStart, triggerCoreStart)
funcNoArgEPICS(triggerCoreStop,  triggerCoreStop)
funcNoArgEPICS(triggerCoreReset, triggerCoreReset)
#undef funcNoArgEPICS
//! \todo no arg function for EPICS: need to implement - //excludeTriggerProcessor empty - testSet -> leaveTestLoopback - readAll ...

/*
static long gtsResetEPICS(subRecord *precord)
{
  if(mySubDebug)
    printf("Record %s called %s(%p/%f) activated if val=1.0\n"
      ,precord->name
      ,__func__
      ,(void*) precord
      ,precord->val
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
}//gtsResetEPICS
*/

#ifndef _X86_64_

#define funcOneArgEPICS(ARG,FCT,VAL) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status = XST_SUCCESS; \
  unsigned long  cardnumber = *((unsigned long *) CARD_NUMBER_ADDRESS); \
 \
  if(precord->val != 1.0) return 0; \
  int val = (int)precord->a; \
  if (mySubDebug) \
       printf("gts %lu is running " #FCT " using " #VAL "=%d\n",  cardnumber, val); \
  status=FCT(val); \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcOneArgEPICS

#else //other ARCH

#define funcOneArgEPICS(ARG,FCT,VAL) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status=0; \
  if(precord->val != 1.0) return 0; \
  int val = (int)precord->a; \
  if(mySubDebug) \
    printf("gts fake: EPICS/%s(subRecord */" #VAL "=%d)\n",__func__,val); \
 \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcOneArgEPICS

#endif //_X86_64_

//funcOneArgEPICS(,,)
funcOneArgEPICS(treeSetup,treeSetup, step)
funcOneArgEPICS(rxSystemIsReady,rxSystemIsReady, transceiver)
funcOneArgEPICS(treeRead,treeRead, transceiver)
funcOneArgEPICS(fineDelaySetNow,fineDelaySetNow, delay)
funcOneArgEPICS(fineDelaySet,fineDelaySet,       delay)
funcOneArgEPICS(coarseDelaySet,coarseDelaySet,   delay)
funcOneArgEPICS(triggerStart,triggerStart, GTStype)
funcOneArgEPICS(printBinary,printBinary, offset)
#undef funcOneArgEPICS
//! \todo 1 arg function for EPICS: need to implement: - progTruncatedIpNumber ->	progHostTruncatedIpNumber - readReg

#ifndef _X86_64_

#define funcTwoArgEPICS(ARG,FCT,VAL1,VAL2) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status = XST_SUCCESS; \
  unsigned long  cardnumber = *((unsigned long *) CARD_NUMBER_ADDRESS); \
 \
  if(precord->val != 1.0) return 0; \
  int val1 = (int)precord->a; \
  int val2 = (int)precord->b; \
  if (mySubDebug) \
       printf("gts %lu is running " #FCT " using " #VAL1 "=%d and " #VAL2 "=%d\n",  cardnumber, val1,val2); \
  status=FCT(val1,val2); \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcTwoArgEPICS

#else //other ARCH

#define funcTwoArgEPICS(ARG,FCT,VAL1,VAL2) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status=0; \
  if(precord->val != 1.0) return 0; \
  int val1 = (int)precord->a; \
  int val2 = (int)precord->b; \
  if(mySubDebug) \
    printf("gts fake: EPICS/%s(subRecord */" #VAL1 "=%d, " #VAL2 "=%d\n",__func__,val1,val2); \
 \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcTwoArgEPICS

#endif //_X86_64_

//funcTwoArgEPICS(,,)
funcTwoArgEPICS(alignSet,alignSet, GTStype,forward)
funcTwoArgEPICS(alignMeas,alignMeas, forward,nmes)
funcTwoArgEPICS(triggerSetup,triggerSetup, GTStype,step)
#undef funcTwoArgEPICS


#ifndef _X86_64_

#define funcThreeArgEPICS(ARG,FCT,VAL1,VAL2,VAL3) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status = XST_SUCCESS; \
  unsigned long  cardnumber = *((unsigned long *) CARD_NUMBER_ADDRESS); \
 \
  if(precord->val != 1.0) return 0; \
  int val1 = (int)precord->a; \
  int val2 = (int)precord->b; \
  int val3 = (int)precord->c; \
  if (mySubDebug) \
       printf("gts %lu is running " #FCT " using " #VAL1 "=%d, " #VAL2 "=%d and " #VAL3 "=%d\n",  cardnumber, val1,val2,val3); \
  status=FCT(val1,val2,val3); \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcThreeArgEPICS

#else //other ARCH

#define funcThreeArgEPICS(ARG,FCT,VAL1,VAL2,VAL3) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status=0; \
  if(precord->val != 1.0) return 0; \
  int val1 = (int)precord->a; \
  int val2 = (int)precord->b; \
  int val3 = (int)precord->c; \
  if(mySubDebug) \
    printf("gts fake: EPICS/%s(subRecord */" #VAL1 "=%d, " #VAL2 "=%d and " #VAL3 "=%d\n",__func__,val1,val2,val3); \
 \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcThreeArgEPICS

#endif //_X86_64_

//funcThreeArgEPICS(,,,)
funcThreeArgEPICS(tdcSet,tdcSet,   meas, debug, delay)
funcThreeArgEPICS(tdcMeas,tdcMeas, meas, debug, nmes)
#undef funcThreeArgEPICS


#ifndef _X86_64_

#define funcFourArgEPICS(ARG,FCT,VAL1,VAL2,VAL3,VAL4) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status = XST_SUCCESS; \
  unsigned long  cardnumber = *((unsigned long *) CARD_NUMBER_ADDRESS); \
 \
  if(precord->val != 1.0) return 0; \
  int val1 = (int)precord->a; \
  int val2 = (int)precord->b; \
  int val3 = (int)precord->c; \
  int val4 = (int)precord->d; \
  if (mySubDebug) \
       printf("gts %lu is running " #FCT " using " #VAL1 "=%d, " #VAL2 "=%d, " #VAL3 "=%d and " #VAL4 "=%d\n",  cardnumber, val1,val2,val3,val4); \
  status=FCT(val1,val2,val3,val4); \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcFourArgEPICS

#else //other ARCH

#define funcFourArgEPICS(ARG,FCT,VAL1,VAL2,VAL3,VAL4) \
static long ARG##EPICS(subRecord *precord) \
{ \
  SUB_DEBUG_PRINT \
  int status=0; \
  if(precord->val != 1.0) return 0; \
  int val1 = (int)precord->a; \
  int val2 = (int)precord->b; \
  int val3 = (int)precord->c; \
  int val4 = (int)precord->d; \
  if(mySubDebug) \
    printf("gts fake: EPICS/%s(subRecord */" #VAL1 "=%d, " #VAL2 "=%d, " #VAL3 "=%d and " #VAL4 "=%d\n",__func__,val1,val2,val3,val4); \
 \
  precord->val = status; \
  return 0; \
}/*ARG##EPICS*/ \
epicsRegisterFunction(ARG##EPICS); \
//funcFourArgEPICS

#endif //_X86_64_

//funcFourArgEPICS(,,,)
funcFourArgEPICS(transCon,       transCon,       con0, con1, con2, con3)
funcFourArgEPICS(transGtsTree,   transGtsTree,   con0, con1, con2, con3)
funcFourArgEPICS(transDigitizer, transDigitizer, con0, con1, con2, con3)
funcFourArgEPICS(alignTdcSet,    alignTdcSet,    GTStype, forward, transceiver, newcal)
#undef funcFourArgEPICS

