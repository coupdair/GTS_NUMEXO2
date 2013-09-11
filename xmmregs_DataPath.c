#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xdebug_l.h"
#include "xmmregs_DataPath.h"

#define DBG XMMR_DATAPATH_DBG

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

static void set_all_txmux(XMMRegs_dp_mux_ctrl *dp_mux, unsigned int val)
{
  dp_mux->mgt_txmux_select = val;
}

static void set_all_rxmux(XMMRegs_dp_mux_ctrl *dp_mux, unsigned char val)
{
  dp_mux->mgt_rxmux_select = val;
}

static int set_sfp_txmux_sync(XMMRegs_dp_mux_ctrl *d, int transceiver, int val)
{
  if (val == SY_SFP) d->sfp_txmux_sync_select = 1;
  else               d->sfp_txmux_sync_select = 0;

  return XST_SUCCESS;
}

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
  return;
}

static void do_one_sweep_nodebug(XMMRegs_dp_tdc_ctrl *dp_tdc)
{
  dp_tdc->pgen_start = 0;
  dp_tdc->pgen_start = 1;
  dp_tdc->pgen_start = 0;
  return; 
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

static int connect_to_digitizer(XMMRegs_dp_mux_ctrl *d, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in connect_to_digitizer");
    return XST_FAILURE; 
  }
  d->mux_digitizer_gts_tree = 1;
  return XST_SUCCESS;
}

static int disconnect_from_digitizer(XMMRegs_dp_mux_ctrl *d, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in disconnect_from_digitizer");
    return XST_FAILURE; 
  }
  d->mux_digitizer_gts_tree = 0;
  return XST_SUCCESS;
}
/*
static int connect_to_trigger_core(XMMRegs_dp_access_mgt_ctrl *dp_access_mgt, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in connect_to_trigger_core");
    return XST_FAILURE; 
  }
  dp_access_mgt->mux = set_bit_to_1(dp_access_mgt->mux, transceiver);
  return XST_SUCCESS;
}
*/
/*
static int disconnect_from_trigger_core(XMMRegs_dp_access_mgt_ctrl *dp_access_mgt, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in disconnect_from_trigger_core");
    return XST_FAILURE; 
  }
  dp_access_mgt->mux = set_bit_to_0(dp_access_mgt->mux, transceiver);
  return XST_SUCCESS;
}
*/
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

int XMMRegs_DataPath_UseSync_Set(XMMRegs *InstancePtr)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  d->use_sync = 1;

  return XST_SUCCESS;
}

int XMMRegs_DataPath_UseSync_UnSet(XMMRegs *InstancePtr)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  d->use_sync = 0;

  return XST_SUCCESS;
}

int XMMRegs_DataPath_UseSync_SfpTxmux_Set(XMMRegs *InstancePtr, int transceiver, int val)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_mux_ctrl *d;
  unsigned int *d_addr;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
  d_addr = (unsigned int *)d;  

  DBG(DBLD, "set_sfp_txmux_sync --->\n");
  DBG(DBLD, "\tvalues before set_sfp_txmux_sync ->\n");
  DBG(DBLD, "\td\t: sfp_txmux_sync\n");
  DBG(DBLD, "\t0x%08X\t: 0x%01X\n", *d_addr, d->sfp_txmux_sync_select);

  if (val == SY_SFP) d->sfp_txmux_sync_select = 1;
  else               d->sfp_txmux_sync_select = 0;

  DBG(DBLD, "\tvalues after set_sfp_txmux_sync ->\n");
  DBG(DBLD, "\t0x%08X\t: 0x%01X\n", *d_addr, d->sfp_txmux_sync_select);  
  DBG(DBLD, "---> set_sfp_txmux_sync |\n");

  return status;
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
int XMMRegs_DataPath_UseSync_MasterToSlave_Set(XMMRegs *InstancePtr, int forward, int transceiver)
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
int XMMRegs_DataPath_UseSync_SyncToSlave_Set(XMMRegs *InstancePtr, int forward, int transceiver )
{
  return XST_SUCCESS;
}

int XMMRegs_DataPath_OneSweep_Do(XMMRegs *InstancePtr)
{
  XMMRegs_dp_tdc_ctrl *dp_tdc;
  void *ba = InstancePtr->BaseAddress;

  dp_tdc = (XMMRegs_dp_tdc_ctrl *) (ba + XMMR_DP_TDC_CTRL_OFFSET);
  do_one_sweep_nodebug(dp_tdc);

  return XST_SUCCESS;
}

int XMMRegs_DataPath_AllTxmux_Set(XMMRegs *InstancePtr, int val)
{
  XMMRegs_dp_mux_ctrl *dp_mux;  
  void *ba;

  if ( (val < DP_TXMUX_REG) || (val > DP_TXMUX_LBK) ) {
    DBG(DBLE, "ERROR : out of range on val in function XMMRegs_DataPath_AllTxmux_Set\n"); 
    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  dp_mux = (XMMRegs_dp_mux_ctrl *) (ba  +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_mux->mgt_txmux_select = val;

  return XST_SUCCESS;
}

int XMMRegs_DataPath_AllRxmux_Set(XMMRegs *InstancePtr, int val)
{
  XMMRegs_dp_mux_ctrl *dp_mux;  
  void *ba;

  if ( (val != DP_RXMUX_MGT) && (val != DP_RXMUX_SFP) ) {
    DBG(DBLE, "ERROR : out of range on val in function XMMRegs_DataPath_AllRxmux_Set\n"); 
    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  dp_mux = (XMMRegs_dp_mux_ctrl *) (ba  +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_mux->mgt_rxmux_select = val;

  return XST_SUCCESS;
}

int XMMRegs_DataPath_CoarseDelay_Set(XMMRegs *InstancePtr, unsigned int delay)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  status |= set_coarse_delay(dp_delay, delay);

  return status;
}

unsigned int XMMRegs_DataPath_CoarseDelay_Read(XMMRegs *InstancePtr)
{
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  return dp_delay->delay;
}

int XMMRegs_DataPath_Digitizer_Connect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  return connect_to_digitizer(d, trans);
}

int XMMRegs_DataPath_Digitizer_Disconnect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_mux_ctrl *d;

  d = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);

  return disconnect_from_digitizer(d, trans);
}
/*
int  XMMRegs_DataPath_TriggerCore_Connect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_access_mgt_ctrl *dp_access_mgt;

  dp_access_mgt = (XMMRegs_dp_access_mgt_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ACCESS_MGT_CTRL_OFFSET);

  return connect_to_trigger_core(dp_access_mgt, trans);
}
*/
/*
int  XMMRegs_DataPath_TriggerCore_DisConnect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_access_mgt_ctrl *dp_access_mgt;

  dp_access_mgt = (XMMRegs_dp_access_mgt_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ACCESS_MGT_CTRL_OFFSET);

  return disconnect_from_trigger_core(dp_access_mgt, trans);
}
*/
/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

int XMMRegs_DataPath_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_mux_ctrl *dp_mux;  
  XMMRegs_dp_delay_ctrl *dp_delay;

  unsigned int *dp_mux_addr;
  unsigned int *dp_delay_addr;

  dp_mux = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);
  dp_mux_addr = (unsigned int *)dp_mux;
  dp_delay_addr = (unsigned int *)dp_delay;

  dp_mux->mgt_txmux_select = 0;
  dp_mux->mgt_rxmux_select = 0;
  dp_mux->use_sync = 0;
  dp_mux->mux_digitizer_gts_tree = 0;
  dp_mux->sfp_txmux_sync_select = 0;
//  disconnect_from_trigger_core(dp_mux, 0);

  status = set_coarse_delay(dp_delay, 0);

  return status;
}

int  XMMRegs_DataPath_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_mux_ctrl *dp_mux;  
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_mux = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  dp_mux->mgt_txmux_select = 0;
  dp_mux->mgt_rxmux_select = 0;
  dp_mux->use_sync = 1;
  dp_mux->mux_digitizer_gts_tree = 0;
  dp_mux->sfp_txmux_sync_select = 1;
//  disconnect_from_trigger_core(dp_mux, 0);

  status = set_coarse_delay(dp_delay, 0);

  return status;
}

int  XMMRegs_DataPath_Start(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  return status;
}

int  XMMRegs_DataPath_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "DataPath_Init --->\n");
  status |= XMMRegs_DataPath_Setup(InstancePtr);
  status |= XMMRegs_DataPath_Start(InstancePtr);
  DBG(DBLD, "---> DataPath_Init |\n");

  return status;
}

int  XMMRegs_DataPath_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_DataPath_Stop(InstancePtr);
  status |= XMMRegs_DataPath_Init(InstancePtr);

  return status;
}

void XMMRegs_DataPath_PrintAll(XMMRegs *InstancePtr)
{
  int i;

  DBG(DBLI, "\nPrintAll of DataPath :---------------------------------------------------------------\n");
  DBG(DBLI, "mgt_regdata_ctrl\t:\t");
  XMMRegs_PrintHex(InstancePtr, XMMR_MGT_REGDATA_CTRL_OFFSET );

  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "dp_mux_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_MUX_CTRL_OFFSET);
/*
  DBG(DBLI, "dp_ulinkmux_ctrl\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_ULINKMUX_CTRL_OFFSET);
  DBG(DBLI, "dp_sync_logic_ctrl\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
*/
/*
  DBG(DBLI, "dp_tdc_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_TDC_CTRL_OFFSET);
*/
  DBG(DBLI, "dp_delay_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_DELAY_CTRL_OFFSET);
/*
  DBG(DBLI, "dp_digitizer_ctrl\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_DIGITIZER_CTRL_OFFSET);
  DBG(DBLI, "dp_access_mgt_ctrl\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_ACCESS_MGT_CTRL_OFFSET);
*/
  return;
}

#undef DBG
