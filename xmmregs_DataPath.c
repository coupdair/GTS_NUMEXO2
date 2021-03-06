#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xdebug_l.h"
#include "xmmregs_DataPath.h"

#define DBG XMMR_DATAPATH_DBG

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

static int set_coarse_delay(XMMRegs_dp_delay_ctrl *dp_delay, unsigned int delay)
{
  if ( delay >= 256 ) {
    DBG(DBLE, "ERROR : out of range error in function set_coarse_delay\n"); 
    DBG(DBLE, "the coarse delay should be smaller than 256 clock cycles\n");
    return XST_FAILURE;
  }

  dp_delay->delay = delay;

  return XST_SUCCESS; 
}

static void init_dp_tdc(XMMRegs_dp_tdc_ctrl *dp_tdc)
{
  dp_tdc->rst_pdet  = 0;
  dp_tdc->pgen_cont = 0;
  dp_tdc->pgen_start = 0;
  dp_tdc->tdc_stop_sync_select = TRANSCEIVER_1; /* TDC_STOP waits signal from TRANSCEIVER_1 in sync mode */
  dp_tdc->tdc_stop_select = 0;
  dp_tdc->tdc_start_select = 0; 
}

static void do_one_sweep_nodebug(XMMRegs_dp_tdc_ctrl *dp_tdc)
{
  dp_tdc->pgen_start = 0;
  dp_tdc->pgen_start = 1;
  dp_tdc->pgen_start = 0;
}

static int set_tdc_stop_sync(XMMRegs_dp_tdc_ctrl *dp_tdc, int transceiver)
{
  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in set_tdc_stop_sync\n");
    return XST_FAILURE; 
  }
  dp_tdc->tdc_stop_sync_select = transceiver;
  return XST_SUCCESS;
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

int XMMRegs_DataPath_MuxClkSync_Set(XMMRegs *InstancePtr, unsigned int val)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_mux_ctrl *d;

  if ( val > 1 ) {
    DBG(DBLE, "ERROR : ouf of range on val in function XMMRegs_DataPath_MuxClkSync_Set\n")
    return XST_FAILURE;
  }

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
  d->mux_clk_sync = val;
  
  return status;
}

void XMMRegs_DataPath_UseSync_Set(XMMRegs *InstancePtr)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  d->use_sync = 1;
}

void XMMRegs_DataPath_UseSync_UnSet(XMMRegs *InstancePtr)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  d->use_sync = 0;
}

/* The signal comes from the master SFP and flows back to the master SFP.
parameter : forward -> in the forward direction, it indicates if the MGT is used or bypassed
Note that, in the backward direction, the MGT is always bypassed */
int XMMRegs_DataPath_UseSync_MasterToMaster_Set(XMMRegs *InstancePtr, int forward)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  d->use_sync = 1; 

  switch (forward) {

    case USE_MGT :
      d->sfp_txmux_sync_select = 0;
      break;

    case BYPASS_MGT :
      d->sfp_txmux_sync_select = 1;
      break;

    default :
      DBG(DBLE, "ERROR : out of range for forward in fucntion XMMRegs_DataPath_UseSync_MasterToMaster_Set\n");
      status = XST_FAILURE;
      break;
  }

  return status;
}

/* FANIN-FANOUT only : The signal comes from the master SFP and flows to a slave SFP.
It comes back from the same SFP and flows back to the master SFP
parameter : forward -> in the forward direction, it indicates if the MGT is used or bypassed 
Note that, in the backward direction, the MGT is always bypassed */
int XMMRegs_DataPath_UseSync_MasterToSlave_Set(XMMRegs *InstancePtr, int forward)
{
  return XST_SUCCESS;
}

/* ROOT only : The signal comes from the sync wire and flows to a slave SFP.
It comes back from the same SFP
parameter : forward -> in the forward direction, it indicates if the MGT is used or bypassed.
Note that, in the backward direction, the MGT is always bypassed 
This function will be used on the root GTS
tdc_stop_sync is also set
*/
int XMMRegs_DataPath_UseSync_SyncToSlave_Set(XMMRegs *InstancePtr, int forward)
{
  return XST_SUCCESS;
}

void XMMRegs_DataPath_OneSweep_Do(XMMRegs *InstancePtr)
{
  XMMRegs_dp_tdc_ctrl *dp_tdc;
  void *ba = InstancePtr->BaseAddress;

  dp_tdc = (XMMRegs_dp_tdc_ctrl *)(ba + XMMR_DP_TDC_CTRL_OFFSET);
  do_one_sweep_nodebug(dp_tdc);
}

void XMMRegs_DataPath_AllTxmux_Set(XMMRegs *InstancePtr, int val)
{
  XMMRegs_dp_mux_ctrl *dp_mux;  
  void *ba;

  ba = InstancePtr->BaseAddress;
  dp_mux = (XMMRegs_dp_mux_ctrl *)(ba  +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_mux->mgt_txmux_select = val;
}

void XMMRegs_DataPath_AllRxmux_Set(XMMRegs *InstancePtr, int val)
{
  XMMRegs_dp_mux_ctrl *dp_mux;  
  void *ba;

  ba = InstancePtr->BaseAddress;
  dp_mux = (XMMRegs_dp_mux_ctrl *)(ba  +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_mux->mgt_rxmux_select = val;
}

int XMMRegs_DataPath_CoarseDelay_Set(XMMRegs *InstancePtr, unsigned int delay)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  return set_coarse_delay(dp_delay, delay);
}

unsigned int XMMRegs_DataPath_CoarseDelay_Read(XMMRegs *InstancePtr)
{
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  return dp_delay->delay;
}

void XMMRegs_DataPath_Digitizer_Connect(XMMRegs *InstancePtr)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  d->mux_digitizer_gts_tree = 1;
}

void XMMRegs_DataPath_Digitizer_Disconnect(XMMRegs *InstancePtr)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  d->mux_digitizer_gts_tree = 0;
}

/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

int XMMRegs_DataPath_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_mux_ctrl *dp_mux;  
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_mux = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  dp_mux->mgt_txmux_select = 0;
  dp_mux->mgt_rxmux_select = 0;
  dp_mux->use_sync = 0;
  dp_mux->mux_digitizer_gts_tree = 0;
  dp_mux->sfp_txmux_sync_select = 0;

  return set_coarse_delay(dp_delay, 0);
}

int XMMRegs_DataPath_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_mux_ctrl *dp_mux;  
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_mux = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  dp_mux->mgt_txmux_select = 0;
  dp_mux->mgt_rxmux_select = 0;
  dp_mux->use_sync = 0;
  dp_mux->mux_digitizer_gts_tree = 0;
  dp_mux->sfp_txmux_sync_select = 0;

  return set_coarse_delay(dp_delay, 0);
}

void XMMRegs_DataPath_Start(XMMRegs *InstancePtr)
{
}

int XMMRegs_DataPath_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "DataPath_Init --->\n");
  status = XMMRegs_DataPath_Setup(InstancePtr);
  XMMRegs_DataPath_Start(InstancePtr);
  DBG(DBLD, "---> DataPath_Init |\n");

  return status;
}

int XMMRegs_DataPath_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_DataPath_Stop(InstancePtr);
  status |= XMMRegs_DataPath_Init(InstancePtr);

  return status;
}

void XMMRegs_DataPath_PrintAll(XMMRegs *InstancePtr)
{
  DBG(DBLI, "\nPrintAll of DataPath :---------------------------------------------------------------\n");
  DBG(DBLI, "mgt_regdata_ctrl\t:\t");
  XMMRegs_PrintHex(InstancePtr, XMMR_MGT_REGDATA_CTRL_OFFSET );
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "dp_mux_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_MUX_CTRL_OFFSET);
  DBG(DBLI, "dp_delay_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_DELAY_CTRL_OFFSET);
}

#undef DBG
