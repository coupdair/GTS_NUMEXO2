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
    return XMMRegs_DataPath_UseSync_MasterToSlave_Set(&XMMRegsDriver, forward);
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
    return XMMRegs_DataPath_UseSync_SyncToSlave_Set(&XMMRegsDriver, forward);
  }
}

/************************************************************

		  HIGH LEVEL FUNCTIONS

************************************************************/

int muxSyncSet(int GTStype, int forward)
{
  int status = XST_SUCCESS;
  int t;

  XMMRegs_DataPath_UseSync_Set(&XMMRegsDriver);

  XMMRegs_DataPath_Digitizer_Disconnect(&XMMRegsDriver);

  switch (GTStype) {
    case ROOT :
      status = syncToSlaveSyncSet(forward, 0);
      break;
    case FANIN_FANOUT :
      status = masterToSlaveSyncSet(forward, 0);
      break;
    case LEAVE :
      status = masterToMasterSyncSet(forward);
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function muxSyncSet\n");
      status = XST_FAILURE;
      break; 
  }

  return status;
}

int allFifoMuxPathSet(int GTStype)
{
  int status = XST_SUCCESS;

  XMMRegs_DataPath_UseSync_UnSet(&XMMRegsDriver);
  XMMRegs_DataPath_AllRxmux_Set(&XMMRegsDriver, 0); // NUMEXO2

  switch (GTStype) {
    case ROOT :
      XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, DP_TXMUX_TST);
      break;
    case FANIN_FANOUT :
      XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, DP_TXMUX_MST);
      break;
    case LEAVE :
      XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, 1); // NUMEXO2
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function triggerMuxPathSet\n");
      status = XST_FAILURE;
      break; 
  }

  return status;
}

int allRegMuxPathSet(void)
{
  int status = XST_SUCCESS;

  XMMRegs_DataPath_UseSync_UnSet(&XMMRegsDriver);
  XMMRegs_DataPath_AllTxmux_Set(&XMMRegsDriver, 0);
  XMMRegs_DataPath_AllRxmux_Set(&XMMRegsDriver, 0);

  return status;
}

#undef DBG
