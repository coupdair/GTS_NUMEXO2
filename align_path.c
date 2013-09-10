#include "align.h"
#include "debug.h"
#include "xmmregs.h"
#include "xmmregs_i.h"

#define DBG DBG_ALIGN_PATH

/************************************************************

		  LOW LEVEL FUNCTIONS

************************************************************/

/* the Datapath muxes are set depending if the transceiver is or not in the GTS tree.
Set the Status unsigned int vector in the XMMRegsDriver structure accordingly */
int doesAlignPathApplyToTransceiver(int transceiver)
{
  int bool = TRANSCEIVER_IS_IN_GTS_TREE;

  bool &= XMMRegs_IsTransceiverInGtsTree(&XMMRegsDriver, transceiver);

  return bool;
}

/* no check here, as the function can be used even if the transceiver is not connected */
int txMuxSfpSet(int transceiver, int val)
{
  return XMMRegs_DataPath_UseSync_SfpTxmux_Set(&XMMRegsDriver, transceiver, val);
}
/*
int txMuxMgtSet(int transceiver, int val)
{
  return XMMRegs_DataPath_UseSync_MgtTxmux_Set(&XMMRegsDriver, transceiver, val);
}
*/
int masterToMasterSyncSet(int forward)
{
  if ( (!doesAlignPathApplyToTransceiver(MASTER_TRANSCEIVER) ) ) 
  {
    DBG(DBLE, "ERROR : no slave transceiver in GTS tree in fucntion masterToMasterSet\n");
    return XST_FAILURE;
  }
  else
  {
    return XMMRegs_DataPath_UseSync_MasterToMaster_Set(&XMMRegsDriver, forward);
  }
}

int masterToSlaveSyncSet(int forward, int transceiver)
{
  if ( !doesAlignPathApplyToTransceiver(transceiver) ) 
  {
    DBG(DBLE, "ERROR : transceiver not in GTS tree, in function masterToSlaveSet\n");
    return XST_FAILURE;
  }
  else
  {
    return XMMRegs_DataPath_UseSync_MasterToSlave_Set(&XMMRegsDriver, forward, transceiver);
  }
}

int syncToSlaveSyncSet(int forward, int transceiver)
{
  if ( !doesAlignPathApplyToTransceiver(transceiver) ) 
  {
    DBG(DBLE, "ERROR : transceiver not in GTS tree, in function syncToSlaveSet\n");
    return XST_FAILURE;
  }
  else
  {
    return XMMRegs_DataPath_UseSync_SyncToSlave_Set(&XMMRegsDriver, forward, transceiver);
  }
}

/************************************************************

		  HIGH LEVEL FUNCTIONS

************************************************************/

int muxSyncSet(int GTStype, int forward, int transceiver)
{
  int status = XST_SUCCESS;
  int t;

  status |= XMMRegs_DataPath_UseSync_Set(&XMMRegsDriver);
  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_3; t++) {
      XMMRegs_DataPath_Digitizer_Disconnect(&XMMRegsDriver, t);
  }

  switch (GTStype) {
    case ROOT :
      status |= syncToSlaveSyncSet(forward, transceiver);
      break;
    case FANIN_FANOUT :
      status |= masterToSlaveSyncSet(forward, transceiver);
      break;
    case LEAVE :
      status |= masterToMasterSyncSet(forward);
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function muxSyncSet\n");
      status |= XST_FAILURE;
      break; 
  }

  return status;
}

int allFifoMuxPathSet(int GTStype)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_DataPath_UseSync_UnSet(&XMMRegsDriver);
  status |= XMMRegs_DataPath_AllRxmux_Set(&XMMRegsDriver, DP_RXMUX_MGT);

  switch (GTStype) {
    case ROOT :
      status |= XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, DP_TXMUX_TST);
      break;
    case FANIN_FANOUT :
      status |= XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, DP_TXMUX_MST);
      break;
    case LEAVE :
      status |= XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, DP_TXMUX_MST);
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function triggerMuxPathSet\n");
      status |= XST_FAILURE;
      break; 
  }

  return status;
}

int allRegMuxPathSet(void)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_DataPath_UseSync_UnSet(&XMMRegsDriver);
  status |= XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, DP_TXMUX_REG);
  status |= XMMRegs_DataPath_AllRxmux_Set(&XMMRegsDriver, DP_RXMUX_MGT);

  return status;
}

#undef DBG
