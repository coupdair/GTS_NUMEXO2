#include "align.h"
#include "xmmregs_i.h"
#include "xdebug_i.h"
#include "xtdc.h"
#include "debug.h"

#define DBG DBG_ALIGN

unsigned int alignModeRead(void)
{
  return XMMRegs_AlignMode_Read(&XMMRegsDriver); 
}

int includeCarrierForAlign(void)
{
  return XMMRegs_AlignMode_Set(&XMMRegsDriver, CARRIER_INCLUDED); 
}

int excludeCarrierForAlign(void)
{
  return XMMRegs_AlignMode_Set(&XMMRegsDriver, CARRIER_EXCLUDED); 
}

int alignStart(void)
{
  gtsReadyUnSet();

  return XST_SUCCESS;
}

/************************************************************
alignSet --
sets the clocks and the external muxes correctly before tdc measurements
its has to be called before alignTdcSet
forward : USE_MGT=1 or BYPASS_MGT=0 
GTStype : either ROOT=1, or FANIN_FANOUT=2, or LEAVE=3
************************************************************/
int alignSet(int GTStype, int forward)
{
  int status = XST_SUCCESS;
  int usrclk;

  if ( (GTStype != ROOT) && (GTStype != FANIN_FANOUT) && (GTStype != LEAVE) ) {
    DBG(DBLE, "ERROR : out of range on GTStype in function alignSet");
    return XST_FAILURE; 
  }

  if ( (forward != USE_MGT) && (forward != BYPASS_MGT) ) {
    DBG(DBLE, "ERROR : out of range on forward in function alignSet");
    return XST_FAILURE; 
  }

  DBG(DBLI, "GTS type : ");  

  switch (GTStype) {

    case ROOT :

      DBG(DBLI, "ROOT\n");
      usrclk = LOCAL_CLK;
      break;

    case FANIN_FANOUT :

      DBG(DBLI, "FANIN_FANOUT\n");
      if ( forward == USE_MGT ) 
        usrclk = RECOVERED_CLK;
      else /* forward = BYPASS_MGT */
        usrclk = LOCAL_CLK; 
      break;

    case LEAVE :

      DBG(DBLI, "LEAF\n");
      if ( alignModeRead() == CARRIER_INCLUDED) {
        if ( forward == USE_MGT ) 
          usrclk = MICTOR_CLK;
        else /* forward = BYPASS_MGT */
          usrclk = LOCAL_CLK; 
      }
      else { /* carrier is excluded */
        if ( forward == USE_MGT ) 
          usrclk = RECOVERED_CLK;
        else /* forward = BYPASS_MGT */
          usrclk = LOCAL_CLK; 
      }
      break;

    default :
      DBG(DBLE, "ERROR : overflow on GTStype in alignSet\n");
      return XST_FAILURE;
  }

  status  = muxExtSet(GTStype, USE_MGT, BYPASS_MGT); // NUMEXO2 LEAF : RX external mux is always 00
  mgtDataCommaSet();
  status |= clkSet(GTStype, usrclk, forward, BYPASS_MGT, FORCE_SET, BLOCKING);

  return status;
}

/************************************************************
  alignTdcSet -- 
Sets DataPath and the tdc correctly before the tdc measurements
GTStype : either ROOT=1, or FANIN_FANOUT=2, or LEAVE=3
forward : USE_MGT=1 or BYPASS_MGT=0
transceiver : TRANSCEIVER_0=0, TRANSCEIVER_1=1, transceiver_2=2, TRANSCEIVER_3=3 
if newcal = DO_NEW_CAL, it reconfigures the TDC on the ROOT node
************************************************************/
int alignTdcSet(int GTStype, int forward, int transceiver, int newcal)
{
  int status = XST_SUCCESS;
  int measure;
  int debug = NO_TDC_DEBUG;
  int delay = DELAY_TDC_DEBUG_DEFAULT;

  if ( (GTStype != ROOT) && (GTStype != FANIN_FANOUT) && (GTStype != LEAVE) ) {
    DBG(DBLE, "ERROR : out of range on GTStype in function alignTdcSet");
    return XST_FAILURE; 
  }

  if ( (forward != USE_MGT) && (forward != BYPASS_MGT) ) {
    DBG(DBLE, "ERROR : out of range on forward in function alignTdcSet");
    return XST_FAILURE; 
  }

  if (forward == USE_MGT) {
    measure = MEAS2;
  }
  else { /* forward == BYPASS_MGT */
    measure = MEAS1;
  }

  status  = XMMRegs_DataPath_AllRxmux_Set(&XMMRegsDriver, 1); // NUMEXO2
  status |= XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, 5); // NUMEXO2

  status = muxSyncSet(GTStype, forward);

  if ( (GTStype == ROOT) && (newcal == DO_NEW_CAL) ) {
    status |= tdcSet(measure, debug, delay);
  }
  else {
    status |= tdcStop();
  }

  return status;
}

/************************************************************
alignMeas --
can be used only on the root node 
forward = either USE_MGT or BYPASS_MGT in the forward direction
nmes : the number of tdc measurements
************************************************************/
int alignMeas(int forward, int nmes)
{
  int measure; 

  if ( (forward != USE_MGT) && (forward != BYPASS_MGT) ) {
    DBG(DBLE, "ERROR : out of range on forward in function alignMeas");
    return XST_FAILURE; 
  }

  if (forward == USE_MGT) {
    measure = MEAS2;
  }
  else { /* forward == BYPASS_MGT */
    measure = MEAS1;
  }

  tdcMeas(measure, NO_TDC_DEBUG, nmes);

  return XST_SUCCESS;
}

#undef DBG
