#include "align.h"
#include "debug.h"
#include "xmmregs.h"
#include "xmmregs_i.h"

#define DBG DBG_ALIGN_SFP

/************************************************************

		  LOW LEVEL FUNCTIONS

************************************************************/

/* the external muxes are set depending if the transceiver is or not in the GTS tree.
Note that being in the GTS tree means:
- being connected
- being connected to the GTS tree
Set the Status u32 vector in the XMMRegsDriver structure accordingly */
int doesAlignSfpApplyToTransceiver(int transceiver)
{
  int bool = TRANSCEIVER_IS_IN_GTS_TREE;

  bool &= XMMRegs_IsTransceiverInGtsTree(&XMMRegsDriver, transceiver);

  return bool;
}

int txMuxExtSet(int transceiver, int mgt_sfp)
{
  if ( doesAlignSfpApplyToTransceiver(transceiver) ) 
    return XMMRegs_MuxInOut_ExternalTxMux_Set(&XMMRegsDriver, transceiver, mgt_sfp);
  else 
    return XMMRegs_MuxInOut_ExternalTxMux_Set(&XMMRegsDriver, transceiver, BYPASS_MGT);
}

int rxMuxExtSet(int transceiver, int mgt_sfp)
{
  if ( doesAlignSfpApplyToTransceiver(transceiver) ) 
    return XMMRegs_MuxInOut_ExternalRxMux_Set(&XMMRegsDriver, transceiver, mgt_sfp);
  else 
    return XMMRegs_MuxInOut_ExternalRxMux_Set(&XMMRegsDriver, transceiver, BYPASS_MGT);
}

int root_MgtForwardMgtBackward_MuxExtSet(void)
{
  int t;

/*  txMuxExtSet(MASTER_TRANSCEIVER, BYPASS_MGT);
  rxMuxExtSet(MASTER_TRANSCEIVER, BYPASS_MGT); */
  /* the following is done to open aurora towards the trigger processor */
  XMMRegs_MuxInOut_ExternalTxMux_Set(&XMMRegsDriver, MASTER_TRANSCEIVER, USE_MGT);
  XMMRegs_MuxInOut_ExternalRxMux_Set(&XMMRegsDriver, MASTER_TRANSCEIVER, USE_MGT);
  /* the following is done for the links of the GTS tree */
/*
  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_3; t++) {
    if (t != MASTER_TRANSCEIVER) {
      txMuxExtSet(t, USE_MGT);
      rxMuxExtSet(t, USE_MGT);
    }
  }
*/
  return XST_SUCCESS;
}

int root_MgtForwardNoMgtBackward_MuxExtSet(void)
{
  int t;

  txMuxExtSet(MASTER_TRANSCEIVER, BYPASS_MGT);
  rxMuxExtSet(MASTER_TRANSCEIVER, BYPASS_MGT);
/*
  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_3; t++) {
    if (t != MASTER_TRANSCEIVER) {
      txMuxExtSet(t, USE_MGT);
      rxMuxExtSet(t, BYPASS_MGT);
    }
  }
*/
  return XST_SUCCESS;
}

int root_NoMgtForwardNoMgtBackward_MuxExtSet(void)
{
  int t;

  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_0; t++) {
      txMuxExtSet(t, BYPASS_MGT);
      rxMuxExtSet(t, BYPASS_MGT);
  }

  return XST_SUCCESS;
}

int faninfanout_MgtForwardMgtBackward_MuxExtSet(void)
{
  int t;

  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_0; t++) {
      txMuxExtSet(t, USE_MGT);
      rxMuxExtSet(t, USE_MGT);
  }

  return XST_SUCCESS;
}

int faninfanout_MgtForwardNoMgtBackward_MuxExtSet(void)
{
  int t;

  txMuxExtSet(MASTER_TRANSCEIVER, BYPASS_MGT);
  rxMuxExtSet(MASTER_TRANSCEIVER, USE_MGT);
/*
  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_3; t++) {
    if (t != MASTER_TRANSCEIVER) {
      txMuxExtSet(t, USE_MGT);
      rxMuxExtSet(t, BYPASS_MGT);
    }
  }
*/
  return XST_SUCCESS;
}

int faninfanout_NoMgtForwardNoMgtBackward_MuxExtSet(void)
{
  int t;

  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_0; t++) {
      txMuxExtSet(t, BYPASS_MGT);
      rxMuxExtSet(t, BYPASS_MGT);
  }

  return XST_SUCCESS;
}

int leave_MgtForwardMgtBackward_MuxExtSet(void)
{
  int t;

  txMuxExtSet(MASTER_TRANSCEIVER, USE_MGT);
  rxMuxExtSet(MASTER_TRANSCEIVER, USE_MGT);
/*
  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_3; t++) {
    if (t != MASTER_TRANSCEIVER) {
      txMuxExtSet(t, BYPASS_MGT);
      rxMuxExtSet(t, BYPASS_MGT);
    }
  }
*/
  return XST_SUCCESS;
}

int leave_MgtForwardNoMgtBackward_MuxExtSet(void)
{
  int t;
  
  txMuxExtSet(MASTER_TRANSCEIVER, BYPASS_MGT);
  rxMuxExtSet(MASTER_TRANSCEIVER, USE_MGT);
/*
  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_3; t++) {
    if (t != MASTER_TRANSCEIVER) {
      txMuxExtSet(t, BYPASS_MGT);
      rxMuxExtSet(t, BYPASS_MGT);
    }
  }
*/
  return XST_SUCCESS;
}

int leave_NoMgtForwardNoMgtBackward_MuxExtSet(void)
{
  int t;

  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_0; t++) {
    txMuxExtSet(t, BYPASS_MGT);
    rxMuxExtSet(t, BYPASS_MGT);
  }

  return XST_SUCCESS;
}

int rootMuxExtSet(int forward, int backward)
{
  int status = XST_SUCCESS;

  if      ( (forward == USE_MGT) && (backward == USE_MGT) ) {
    status |= root_MgtForwardMgtBackward_MuxExtSet();
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status |= root_MgtForwardNoMgtBackward_MuxExtSet();
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : function rootMuxExtSet : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status |= root_NoMgtForwardNoMgtBackward_MuxExtSet();
  }
  else {
    DBG(DBLE, "ERROR : out of range error on forward or backward values in function rootMuxExtSet\n");
    status = XST_FAILURE;
  }

  return status;
}

int faninfanoutMuxExtSet(int forward, int backward)
{
  int status = XST_SUCCESS;

  if      ( (forward == USE_MGT) && (backward == USE_MGT) ) {
    status |= faninfanout_MgtForwardMgtBackward_MuxExtSet();
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status |= faninfanout_MgtForwardNoMgtBackward_MuxExtSet();
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : function faninfanoutMuxExtSet : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status |= faninfanout_NoMgtForwardNoMgtBackward_MuxExtSet();
  }
  else {
    DBG(DBLE, "ERROR : out of range error on forward or backward values in function faninfanoutMuxExtSet\n");
    status = XST_FAILURE;
  }

  return status;
}

int leaveMuxExtSet(int forward, int backward)
{
  int status = XST_SUCCESS;

  if      ( (forward == USE_MGT) && (backward == USE_MGT) ) {
    status |= leave_MgtForwardMgtBackward_MuxExtSet();
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status |= leave_MgtForwardNoMgtBackward_MuxExtSet();
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : function leaveMuxExtSet : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status |= leave_NoMgtForwardNoMgtBackward_MuxExtSet();
  }
  else {
    DBG(DBLE, "ERROR : out of range error on forward or backward values in function leaveMuxExtSet\n");
    status = XST_FAILURE;
  }

  return status;
}

/************************************************************

		HIGH LEVEL SET FUNCTIONS

************************************************************/

int muxExtSet(int GTStype, int forward, int backward)
{
  int status = XST_SUCCESS;

  switch (GTStype) {
    case ROOT :
      status |= rootMuxExtSet(forward, backward);
      break;
    case FANIN_FANOUT :
      status |= faninfanoutMuxExtSet(forward, backward);
      break;
    case LEAVE :
      status |= leaveMuxExtSet(forward, backward);
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function muxExtSet\n");
      status |= XST_FAILURE;
      break;
  }

  return status;
}

#undef DBG
