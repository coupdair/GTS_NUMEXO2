#include "xdebug_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"

#define DBG XMMR_TDCDEBUG_DBG

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

static void do_one_sweep_debug(XMMRegs_tdc_debug_ctrl *t)
{
  t->one_sweep   = 0;
  t->one_sweep   = 1;
  return;
}

static void set_delay(XMMRegs_tdc_debug_ctrl *t, unsigned int delay)
{

  if ( delay >= 0x4000 ) {
    DBG(DBLE, "ERROR : overflow in function set_delay\n");
    DBG(DBLE, "value should be <= 2^18\n") 
  }
  t->delay       = delay;
  return;
}

/* running mode mode : the delay between the start and the stop signal
is determined by the DataPath */
static void ctrl_to_DataPath(XMMRegs_tdc_debug_ctrl *t)
{
  t->one_sweep   = 0; 
  t->ctrl_enable = 0;
  return;
}

/* debug mode : the delay between the start and the stop signal
is controlled by the start pulse and the setting coming from the uP */
static void ctrl_to_uP(XMMRegs_tdc_debug_ctrl *t)
{
  t->one_sweep   = 0; 
  t->ctrl_enable = 1;
  return;
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

int XMMRegs_TdcDebug_OneSweep_Do(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_debug_ctrl *t;

  t = (XMMRegs_tdc_debug_ctrl *)(ba + XMMR_TDC_DEBUG_CTRL_OFFSET);
  do_one_sweep_debug(t);

  return XST_SUCCESS;
}

int XMMRegs_TdcDebug_Delay_Set(XMMRegs *InstancePtr, unsigned int delay)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_debug_ctrl *t;

  t = (XMMRegs_tdc_debug_ctrl *)(ba + XMMR_TDC_DEBUG_CTRL_OFFSET);
  set_delay(t, delay);

  return XST_SUCCESS;
}

int XMMRegs_TdcDebug_DebugMode_Launch(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_debug_ctrl *t;

  t = (XMMRegs_tdc_debug_ctrl *)(ba + XMMR_TDC_DEBUG_CTRL_OFFSET);

  ctrl_to_uP(t);

  return XST_SUCCESS;
}

int XMMRegs_TdcDebug_DebugMode_Remove(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_debug_ctrl *t;

  t = (XMMRegs_tdc_debug_ctrl *)(ba + XMMR_TDC_DEBUG_CTRL_OFFSET);

  ctrl_to_DataPath(t);

  return XST_SUCCESS;
}

/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

int XMMRegs_TdcDebug_Setup(XMMRegs *InstancePtr)
{
  XMMRegs_tdc_debug_ctrl *t;
  void *ba = InstancePtr->BaseAddress;

  t = (XMMRegs_tdc_debug_ctrl *)(ba + XMMR_TDC_DEBUG_CTRL_OFFSET);
  set_delay(t, DELAY_TDC_DEBUG_DEFAULT);
  ctrl_to_DataPath(t);

  return XST_SUCCESS;
}

int XMMRegs_TdcDebug_Stop(XMMRegs *InstancePtr)
{
  XMMRegs_tdc_debug_ctrl *t;
  void *ba = InstancePtr->BaseAddress;

  t = (XMMRegs_tdc_debug_ctrl *)(ba + XMMR_TDC_DEBUG_CTRL_OFFSET);
  set_delay(t, DELAY_TDC_DEBUG_DEFAULT);
  ctrl_to_DataPath(t);

  return XST_SUCCESS;
}

int XMMRegs_TdcDebug_Start(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  return status;
}

int XMMRegs_TdcDebug_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "TdcDebug_Init --->\n");
  status |= XMMRegs_TdcDebug_Setup(InstancePtr);
  status |= XMMRegs_TdcDebug_Start(InstancePtr);
  DBG(DBLD, "---> TdcDebug_Init |\n");

  return status;
}

int XMMRegs_TdcDebug_Reset(XMMRegs *InstancePtr)
{
  /* local variables */
  int status = XST_SUCCESS;

  status |= XMMRegs_TdcDebug_Stop(InstancePtr);
  status |= XMMRegs_TdcDebug_Init(InstancePtr);

  return status;
}

void XMMRegs_TdcDebug_PrintAll(XMMRegs *InstancePtr)
{
  DBG(DBLI, "\nPrintAll of TdcDebug :---------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "tdc_debug_ctrl\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_TDC_DEBUG_CTRL_OFFSET);

  return;
}

#undef DBG
