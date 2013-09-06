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
  dp_mux->txmux0 = val;
  dp_mux->txmux1 = val;
  dp_mux->txmux2 = val;
  dp_mux->txmux3 = val; 
  return;
}

static void set_all_rxmux(XMMRegs_dp_mux_ctrl *dp_mux, unsigned char val)
{
  dp_mux->rxmux0 = val;
  dp_mux->rxmux1 = val;
  dp_mux->rxmux2 = val;
  dp_mux->rxmux3 = val; 
  return;
}

static int set_sfp_txmux_sync(XMMRegs_dp_sync_logic_ctrl *dp_sync, int transceiver, int val)
{
  int status = XST_SUCCESS;

  switch (transceiver) {
    case TRANSCEIVER_0 :
      switch (val) {
        case SY_SFP : case SY_MGT_MASTER :
          if ( MUX_SYNC(val) )
            dp_sync->sfp_txmux_sync = set_bit_to_1(dp_sync->sfp_txmux_sync, transceiver);
          else
            dp_sync->sfp_txmux_sync = set_bit_to_0(dp_sync->sfp_txmux_sync, transceiver);            
          break;
        default :
          DBG(DBLE, "ERROR : out of range on val in function set_sfp_txmux_sync");
          status |= XST_FAILURE;
          break; 
      }
      break;
    case TRANSCEIVER_1 : case TRANSCEIVER_2 : case TRANSCEIVER_3 :
      switch (val) {
        case SY_SFP_MASTER : case SY_SYNC :
          if ( MUX_SYNC(val) )
            dp_sync->sfp_txmux_sync = set_bit_to_1(dp_sync->sfp_txmux_sync, transceiver);
          else
            dp_sync->sfp_txmux_sync = set_bit_to_0(dp_sync->sfp_txmux_sync, transceiver);            
          break;
        default : 
          DBG(DBLE, "ERROR : out of range on val in function set_sfp_txmux_sync");
          status |= XST_FAILURE;
          break; 
      }
      break;
    default :
      DBG(DBLE, "ERROR : index of transceiver out of range in function set_sfp_txmux_sync\n");
      status |= XST_FAILURE;
      break;
  }

  return status;
}

static int set_mgt_txmux_sync(XMMRegs_dp_sync_logic_ctrl *dp_sync, int transceiver, int val)
{
  int status = XST_SUCCESS;

  switch (transceiver) {
    case TRANSCEIVER_0 :
      DBG(DBLW, "WARNING : set_mgt_txmux_sync has no effect on master transceiver in transmission\n");
      dp_sync->mgt_txmux_sync = set_bit_to_1(dp_sync->sfp_txmux_sync, transceiver);
      break;
    case TRANSCEIVER_1 : case TRANSCEIVER_2 : case TRANSCEIVER_3 :
      switch (val) {
        case SY_MGT_MASTER : case SY_SYNC :
          if ( MUX_SYNC(val) )
            dp_sync->mgt_txmux_sync = set_bit_to_1(dp_sync->mgt_txmux_sync, transceiver);
          else
            dp_sync->mgt_txmux_sync = set_bit_to_0(dp_sync->mgt_txmux_sync, transceiver);
          break;
        default : 
          DBG(DBLE, "ERROR : out of range on val in function set_mgt_txmux_sync");
          status |= XST_FAILURE;
          break; 
      }
      break;
    default :
      DBG(DBLE, "ERROR : index of transceiver out of range in function set_mgt_txmux_sync\n");
      status |= XST_FAILURE;
      break;
  }

  return status;  
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
  
static int use_sync(XMMRegs_dp_sync_logic_ctrl *d) {
  d->use_sync = SY_SYNC_USE;
  return XST_SUCCESS;
}

static int bypass_sync(XMMRegs_dp_sync_logic_ctrl *d) {
  d->use_sync = SY_SYNC_BYPASS;
  return XST_SUCCESS;
}

static int select_sfp_sync(XMMRegs_dp_sync_logic_ctrl *d, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in set_sfp_sync_select");
    return XST_FAILURE; 
  }
  d->sfp_sync = transceiver;
  return XST_SUCCESS;
}

static int connect_to_digitizer(XMMRegs_dp_digitizer_ctrl *dp_dig, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in connect_to_digitizer");
    return XST_FAILURE; 
  }
  dp_dig->mux = set_bit_to_1(dp_dig->mux, transceiver);
  return XST_SUCCESS;
}

static int disconnect_from_digitizer(XMMRegs_dp_digitizer_ctrl *dp_dig, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in disconnect_from_digitizer");
    return XST_FAILURE; 
  }
  dp_dig->mux = set_bit_to_0(dp_dig->mux, transceiver);
  return XST_SUCCESS;
}

static int connect_to_trigger_core(XMMRegs_dp_access_mgt_ctrl *dp_access_mgt, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in connect_to_trigger_core");
    return XST_FAILURE; 
  }
  dp_access_mgt->mux = set_bit_to_1(dp_access_mgt->mux, transceiver);
  return XST_SUCCESS;
}

static int disconnect_from_trigger_core(XMMRegs_dp_access_mgt_ctrl *dp_access_mgt, int transceiver)
{
  if ( (transceiver > TRANSCEIVER_3) || (transceiver < TRANSCEIVER_0) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in disconnect_from_trigger_core");
    return XST_FAILURE; 
  }
  dp_access_mgt->mux = set_bit_to_0(dp_access_mgt->mux, transceiver);
  return XST_SUCCESS;
}
  
/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

int XMMRegs_DataPath_MuxClkSync_Set(XMMRegs *InstancePtr, unsigned int val)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_digitizer_ctrl *dp_digitizer;

  if ( val > 0xF ) {
    DBG(DBLE, "ERROR : ouf of range on val in function XMMRegs_DataPath_MuxClkSync_Set\n")
    return XST_FAILURE;
  }

  dp_digitizer = (XMMRegs_dp_digitizer_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DIGITIZER_CTRL_OFFSET);
  dp_digitizer->mux_clk_sync = val;
  
  return status;
}

int XMMRegs_DataPath_UseSync_Set(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;

  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
  status |= use_sync(dp_sync_logic);

  return status;
}

int XMMRegs_DataPath_UseSync_UnSet(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;

  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
  status |= bypass_sync(dp_sync_logic);

  return status;
}

int XMMRegs_DataPath_UseSync_SfpTxmux_Set(XMMRegs *InstancePtr, int transceiver, int val)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;
  unsigned int *dp_sync_logic_addr;

  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
  dp_sync_logic_addr = (unsigned int *) dp_sync_logic;  

  DBG(DBLD, "set_sfp_txmux_sync --->\n");
  DBG(DBLD, "\tvalues before set_sfp_txmux_sync ->\n");
  DBG(DBLD, "\tdp_sync_logic\t: sfp_txmux_sync\n");
  DBG(DBLD, "\t0x%08X\t: 0x%01X\n", *dp_sync_logic_addr, dp_sync_logic->sfp_txmux_sync);

  status |= set_sfp_txmux_sync(dp_sync_logic, transceiver, val);

  DBG(DBLD, "\tvalues after set_sfp_txmux_sync ->\n");
  DBG(DBLD, "\t0x%08X\t: 0x%01X\n", *dp_sync_logic_addr, dp_sync_logic->sfp_txmux_sync);  
  DBG(DBLD, "---> set_sfp_txmux_sync |\n");

  return status;
}

int XMMRegs_DataPath_UseSync_MgtTxmux_Set(XMMRegs *InstancePtr, int transceiver, int val)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;
  unsigned int *dp_sync_logic_addr;

  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
  dp_sync_logic_addr = (unsigned int *) dp_sync_logic;  

  DBG(DBLD, "set_mgt_txmux_sync --->\n");
  DBG(DBLD, "\tvalues before set_mgt_txmux_sync ->\n");
  DBG(DBLD, "\tdp_sync_logic\t: mgt_txmux_sync\n");
  DBG(DBLD, "\t0x%08X\t: 0x%01X\n", *dp_sync_logic_addr, dp_sync_logic->mgt_txmux_sync);

  status |= set_sfp_txmux_sync(dp_sync_logic, transceiver, val);

  DBG(DBLD, "\tvalues after set_mgt_txmux_sync ->\n");
  DBG(DBLD, "\t0x%08X\t: 0x%01X\n", *dp_sync_logic_addr, dp_sync_logic->mgt_txmux_sync);  
  DBG(DBLD, "---> set_mgt_txmux_sync |\n");

  return status;
}

/* The signal comes from the master SFP and flows back to the master SFP.
parameter : forward -> in the forward direction, it indicates if the MGT is used or bypassed
Note that, in the backward direction, the MGT is always bypassed */
int XMMRegs_DataPath_UseSync_MasterToMaster_Set(XMMRegs *InstancePtr, int forward)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;

  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);

  switch (forward) {

    case USE_MGT :

      status |= use_sync(dp_sync_logic);
      status |= select_sfp_sync(dp_sync_logic, TRANSCEIVER_0);

      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_MGT_MASTER);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);

      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC); 
      break;

    case BYPASS_MGT :

      status |= use_sync(dp_sync_logic);
      status |= select_sfp_sync(dp_sync_logic, TRANSCEIVER_0);

      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_SFP);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);

      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC); 
      break;

    default :

      DBG(DBLE, "ERROR : out of range for forward in fucntion XMMRegs_DataPath_UseSync_MasterToMaster_Set\n");
      status |= XST_FAILURE;
      break;
  }

  return status;
}

/* The signal comes from the master SFP and flows to a slave SFP.
It comes back from the same SFP and flows back to the master SFP
parameter : forward -> in the forward direction, it indicates if the MGT is used or bypassed 
Note that, in the backward direction, the MGT is always bypassed */
int XMMRegs_DataPath_UseSync_MasterToSlave_Set(XMMRegs *InstancePtr, int forward, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;
  int t;

  if ( (transceiver < TRANSCEIVER_1) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in XMMRegs_DataPath_UseSync_MasterToSlave_Set\n");
    return XST_FAILURE; 
  }

  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
  status |= use_sync(dp_sync_logic);

  switch (forward) {

    case USE_MGT :

      status |= select_sfp_sync(dp_sync_logic, transceiver);

      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_SFP);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);

      for (t = TRANSCEIVER_1; t <= TRANSCEIVER_3; t++) {
        if (t == transceiver)
          status |= set_mgt_txmux_sync(dp_sync_logic, t, SY_MGT_MASTER);
        else
          status |= set_mgt_txmux_sync(dp_sync_logic, t, SY_SYNC);
      }
      break;

    case BYPASS_MGT :

      status |= select_sfp_sync(dp_sync_logic, transceiver);

      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_SFP);
      for (t = TRANSCEIVER_1; t <= TRANSCEIVER_3; t++) {
        if (t == transceiver)
          status |= set_sfp_txmux_sync(dp_sync_logic, t, SY_SFP_MASTER);
        else
          status |= set_sfp_txmux_sync(dp_sync_logic, t, SY_SYNC);
      }

      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC); 
      break;

    default :

      DBG(DBLE, "ERROR : out of range for forward in function XMMRegs_DataPath_UseSync_MasterToSlave_Set\n");
      status |= XST_FAILURE;
      break;
  }

  return status;
}

/* The signal comes from the sync wire and flows to a slave SFP.
It comes back from the same SFP
parameter : forward -> in the forward direction, it indicates if the MGT is used or bypassed.
Note that, in the backward direction, the MGT is always bypassed 
This function will be used on the root GTS
tdc_stop_sync is also set
*/
int XMMRegs_DataPath_UseSync_SyncToSlave_Set(XMMRegs *InstancePtr, int forward, int transceiver )
{
  int status = XST_SUCCESS;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;
  XMMRegs_dp_tdc_ctrl *dp_tdc;

  if ( (transceiver < TRANSCEIVER_1) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : transceiver is out of range in XMMRegs_DataPath_UseSync_SyncToSlave_Set\n");
    return XST_FAILURE; 
  }

  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
  dp_tdc = (XMMRegs_dp_tdc_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_TDC_CTRL_OFFSET);
  status |= use_sync(dp_sync_logic);

  switch (forward) {

    case USE_MGT : case BYPASS_MGT :

      status |= select_sfp_sync(dp_sync_logic, TRANSCEIVER_0); /* master not used */

      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_SFP); /* SFP master loopback, irrelevant stuff */
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);

      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
      status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC); 
      break;

    default :

      DBG(DBLE, "ERROR : out of range for forward in function XMMRegs_DataPath_UseSync_SyncToSlave_Set\n");
      status |= XST_FAILURE;
      break;
  }

  status |= set_tdc_stop_sync(dp_tdc, transceiver);

  return status;
}

int  XMMRegs_DataPath_OneSweep_Do(XMMRegs *InstancePtr)
{
  XMMRegs_dp_tdc_ctrl *dp_tdc;
  void *ba = InstancePtr->BaseAddress;

  dp_tdc = (XMMRegs_dp_tdc_ctrl *) (ba + XMMR_DP_TDC_CTRL_OFFSET);
  do_one_sweep_nodebug(dp_tdc);

  return XST_SUCCESS;
}

int  XMMRegs_DataPath_AllTxmux_Set(XMMRegs *InstancePtr, int val)
{
  XMMRegs_dp_mux_ctrl *dp_mux;  
  void *ba;

  if ( (val < DP_TXMUX_REG) || (val > DP_TXMUX_LBK) ) {
    DBG(DBLE, "ERROR : out of range on val in function XMMRegs_DataPath_AllTxmux_Set\n"); 
    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  dp_mux = (XMMRegs_dp_mux_ctrl *) (ba  +  XMMR_DP_MUX_CTRL_OFFSET);
  set_all_txmux(dp_mux, val);

  return XST_SUCCESS;
}

int  XMMRegs_DataPath_AllRxmux_Set(XMMRegs *InstancePtr, int val)
{
  XMMRegs_dp_mux_ctrl *dp_mux;  
  void *ba;

  if ( (val != DP_RXMUX_MGT) && (val != DP_RXMUX_SFP) ) {
    DBG(DBLE, "ERROR : out of range on val in function XMMRegs_DataPath_AllRxmux_Set\n"); 
    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  dp_mux = (XMMRegs_dp_mux_ctrl *) (ba  +  XMMR_DP_MUX_CTRL_OFFSET);
  set_all_rxmux(dp_mux, val);

  return XST_SUCCESS;
}

int  XMMRegs_DataPath_CoarseDelay_Set(XMMRegs *InstancePtr, unsigned int delay)
{
  int status = XST_SUCCESS;
  XMMRegs_dp_delay_ctrl *dp_delay;

  /* takes register addresses */

  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  status |= set_coarse_delay(dp_delay, delay);

  return status;
}

unsigned int  XMMRegs_DataPath_CoarseDelay_Read(XMMRegs *InstancePtr)
{
  XMMRegs_dp_delay_ctrl *dp_delay;

  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);

  return dp_delay->delay;
}

int  XMMRegs_DataPath_Digitizer_Connect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_digitizer_ctrl *dp_dig;

  dp_dig = (XMMRegs_dp_digitizer_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DIGITIZER_CTRL_OFFSET);

  return connect_to_digitizer(dp_dig, trans);
}

int  XMMRegs_DataPath_Digitizer_Disconnect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_digitizer_ctrl *dp_dig;

  dp_dig = (XMMRegs_dp_digitizer_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DIGITIZER_CTRL_OFFSET);

  return disconnect_from_digitizer(dp_dig, trans);
}

int  XMMRegs_DataPath_TriggerCore_Connect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_access_mgt_ctrl *dp_access_mgt;

  dp_access_mgt = (XMMRegs_dp_access_mgt_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ACCESS_MGT_CTRL_OFFSET);

  return connect_to_trigger_core(dp_access_mgt, trans);
}

int  XMMRegs_DataPath_TriggerCore_DisConnect(XMMRegs *InstancePtr, int trans)
{
  XMMRegs_dp_access_mgt_ctrl *dp_access_mgt;

  dp_access_mgt = (XMMRegs_dp_access_mgt_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ACCESS_MGT_CTRL_OFFSET);

  return disconnect_from_trigger_core(dp_access_mgt, trans);
}

/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

int  XMMRegs_DataPath_Setup(XMMRegs *InstancePtr)
{
  /* local variables */
  int status = XST_SUCCESS;
  int trans;
  XMMRegs_dp_mux_ctrl *dp_mux;  
  XMMRegs_dp_ulinkmux_ctrl *dp_ulinkmux;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;
  XMMRegs_dp_delay_ctrl *dp_delay;
  XMMRegs_dp_tdc_ctrl *dp_tdc;
  XMMRegs_mgt_sel_ctrl *mgt_sel;
  XMMRegs_dp_digitizer_ctrl *dp_dig;
  XMMRegs_dp_access_mgt_ctrl *dp_access_mgt;

  unsigned int *dp_mux_addr, *dp_ulinkmux_addr, *dp_sync_logic_addr;
  unsigned int *dp_delay_addr, *dp_tdc_addr;

  /* takes register addresses */

  dp_mux = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
//  dp_ulinkmux = (XMMRegs_dp_ulinkmux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ULINKMUX_CTRL_OFFSET);
  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);  
  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);
  dp_tdc = (XMMRegs_dp_tdc_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_TDC_CTRL_OFFSET) ;
  mgt_sel = (XMMRegs_mgt_sel_ctrl *)(InstancePtr->BaseAddress +  XMMR_MGT_SEL_CTRL_OFFSET) ;
  dp_dig = (XMMRegs_dp_digitizer_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DIGITIZER_CTRL_OFFSET) ;
  dp_access_mgt = (XMMRegs_dp_access_mgt_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ACCESS_MGT_CTRL_OFFSET) ;
  dp_mux_addr = (unsigned int *) dp_mux;
  dp_ulinkmux_addr = (unsigned int *) dp_ulinkmux;
  dp_sync_logic_addr = (unsigned int *) dp_sync_logic;
  dp_delay_addr = (unsigned int *) dp_delay;
  dp_tdc_addr = (unsigned int *) dp_tdc;

  /* debug before initialization of registers */
/*
  DBG(DBLD, "\tAddresses ->\n");
  DBG(DBLD, "\tdp_mux_ctrl\t\t: %p\n", dp_mux);
  DBG(DBLD, "\tdp_ulinkmux_ctrl\t: %p\n", dp_ulinkmux);
  DBG(DBLD, "\tdp_sync_logic_ctrl\t: %p\n", dp_sync_logic);
  DBG(DBLD, "\tbefore DataPath_Setup ->\n");
  DBG(DBLD, "\tdp_mux_ctrl\t: ulinkmux\t: sync_logic\t: delay\t\t: tdc\n");
  DBG(DBLD, "\t0x%08X\t: 0x%08X\t: 0x%08X\t: 0x%08X\t: 0x%08X\n", *dp_mux_addr, *dp_ulinkmux_addr, *dp_sync_logic_addr, *dp_delay_addr, *dp_tdc_addr);
*/
  /* set registers */

  set_all_txmux(dp_mux, DP_TXMUX_REG);
  set_all_rxmux(dp_mux, DP_RXMUX_MGT);
/*
  dp_ulinkmux->ulinkmux = TRANSCEIVER_0;

  status |= bypass_sync(dp_sync_logic);
  status |= select_sfp_sync(dp_sync_logic, TRANSCEIVER_0);

  for (trans = 0; trans < NB_TRANSCEIVER; trans++) {
    status |= disconnect_from_digitizer(dp_dig, trans);
    status |= disconnect_from_trigger_core(dp_access_mgt, trans);
  }

  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_MGT_MASTER);
  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);

//  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_SYNC);
  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);
*/
  status |= set_coarse_delay(dp_delay, 0);
  init_dp_tdc(dp_tdc);

  /* debug after initialization of registers */
/*
  DBG(DBLD, "\tafter DataPath_Setup ->\n");
  DBG(DBLD, "\t0x%08X\t: 0x%08X\t: 0x%08X\t: 0x%08X\t: 0x%08X\n", *dp_mux_addr, *dp_ulinkmux_addr, *dp_sync_logic_addr, *dp_delay_addr, *dp_tdc_addr);
*/
  return status;
}

int  XMMRegs_DataPath_Stop(XMMRegs *InstancePtr)
{
  /* local variables */
  int status = XST_SUCCESS;
  int trans;
  XMMRegs_dp_mux_ctrl *dp_mux;  
  XMMRegs_dp_ulinkmux_ctrl *dp_ulinkmux;
  XMMRegs_dp_sync_logic_ctrl *dp_sync_logic;
  XMMRegs_dp_delay_ctrl *dp_delay;
  XMMRegs_dp_tdc_ctrl *dp_tdc;
  XMMRegs_mgt_sel_ctrl *mgt_sel;
  XMMRegs_dp_digitizer_ctrl *dp_dig;
  XMMRegs_dp_access_mgt_ctrl *dp_access_mgt;

  /* takes register addresses */

  dp_mux = (XMMRegs_dp_mux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_MUX_CTRL_OFFSET);
//  dp_ulinkmux = (XMMRegs_dp_ulinkmux_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ULINKMUX_CTRL_OFFSET);
  dp_sync_logic = (XMMRegs_dp_sync_logic_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);  
  dp_delay = (XMMRegs_dp_delay_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DELAY_CTRL_OFFSET);
  dp_tdc = (XMMRegs_dp_tdc_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_TDC_CTRL_OFFSET) ;
  mgt_sel = (XMMRegs_mgt_sel_ctrl *)(InstancePtr->BaseAddress +  XMMR_MGT_SEL_CTRL_OFFSET) ;
  dp_dig = (XMMRegs_dp_digitizer_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_DIGITIZER_CTRL_OFFSET) ;
  dp_access_mgt = (XMMRegs_dp_access_mgt_ctrl *)(InstancePtr->BaseAddress +  XMMR_DP_ACCESS_MGT_CTRL_OFFSET) ;

  /* set registers */

  set_all_txmux(dp_mux, DP_TXMUX_REG);
  set_all_rxmux(dp_mux, DP_RXMUX_MGT);
/*
  dp_ulinkmux->ulinkmux = TRANSCEIVER_0;

  dp_sync_logic->use_sync = SY_SYNC_USE;
  dp_sync_logic->sfp_sync = TRANSCEIVER_0;

  for (trans = TRANSCEIVER_0; trans < NB_TRANSCEIVER; trans++) {
    status |= disconnect_from_digitizer(dp_dig, trans);
    status |= disconnect_from_trigger_core(dp_access_mgt, trans);
  }

  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_SFP);
  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
  status |= set_sfp_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);

  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_0, SY_SYNC);
  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_1, SY_SYNC);
  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_2, SY_SYNC);
  status |= set_mgt_txmux_sync(dp_sync_logic, TRANSCEIVER_3, SY_SYNC);
*/
  status |= set_coarse_delay(dp_delay, 0);
  init_dp_tdc(dp_tdc);

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
  for (i = 0; i <= 0; i++) {
    DBG(DBLI, "mgt_regdata_ctrl(%d)\t:\t", i);
    XMMRegs_PrintHex(InstancePtr, XMMR_MGT_REGDATA_CTRL_OFFSET + (4*i) );
  }
/*
  for (i = 0; i <= 0; i++) {
    DBG(DBLI, "mgt_rrdata_status(%d, %d)\t:\t", 2*i+1, 2*i);
    XMMRegs_PrintHex(InstancePtr, XMMR_MGT_RRDATA_STATUS_OFFSET + (4*i) );
  }
*/
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "dp_mux_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_MUX_CTRL_OFFSET);
/*
  DBG(DBLI, "dp_ulinkmux_ctrl\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_ULINKMUX_CTRL_OFFSET);
  DBG(DBLI, "dp_sync_logic_ctrl\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_SYNC_LOGIC_CTRL_OFFSET);
*/
  DBG(DBLI, "dp_tdc_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_DP_TDC_CTRL_OFFSET);
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
