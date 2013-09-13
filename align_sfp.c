#include "align.h"
#include "debug.h"
#include "xmmregs.h"
#include "xmmregs_i.h"

#define DBG DBG_ALIGN_SFP

/************************************************************

		  LOW LEVEL FUNCTIONS

************************************************************/

void txMuxExtSet(int transceiver, int b)
{
  XMMRegs_MuxInOut_ExternalTxMux_Set(&XMMRegsDriver, b);
}

void rxMuxExtSet(int transceiver, int b)
{
  XMMRegs_MuxInOut_ExternalRxMux_Set(&XMMRegsDriver, b);
}

void root_MgtForwardMgtBackward_MuxExtSet(void)
{
}

void root_MgtForwardNoMgtBackward_MuxExtSet(void)
{
}

void root_NoMgtForwardNoMgtBackward_MuxExtSet(void)
{
}

void faninfanout_MgtForwardMgtBackward_MuxExtSet(void)
{
  txMuxExtSet(0, 0);
  rxMuxExtSet(0, 0);
}

void faninfanout_MgtForwardNoMgtBackward_MuxExtSet(void)
{
  txMuxExtSet(0, 1);
  rxMuxExtSet(0, 0);
}

void faninfanout_NoMgtForwardNoMgtBackward_MuxExtSet(void)
{
  txMuxExtSet(0, 1);
  rxMuxExtSet(0, 1);
}

void leave_MgtForwardMgtBackward_MuxExtSet(void)
{
  txMuxExtSet(0, 0);
  rxMuxExtSet(0, 0);
}

void leave_MgtForwardNoMgtBackward_MuxExtSet(void)
{
  txMuxExtSet(0, 1);
  rxMuxExtSet(0, 0);
}

void leave_NoMgtForwardNoMgtBackward_MuxExtSet(void)
{
  txMuxExtSet(0, 1);
  rxMuxExtSet(0, 1);
}

int rootMuxExtSet(int forward, int backward)
{
  int status = XST_SUCCESS;

  if      ( (forward == USE_MGT) && (backward == USE_MGT) ) {
    root_MgtForwardMgtBackward_MuxExtSet();
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    root_MgtForwardNoMgtBackward_MuxExtSet();
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : function rootMuxExtSet : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    root_NoMgtForwardNoMgtBackward_MuxExtSet();
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
    faninfanout_MgtForwardMgtBackward_MuxExtSet();
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    faninfanout_MgtForwardNoMgtBackward_MuxExtSet();
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : function faninfanoutMuxExtSet : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    faninfanout_NoMgtForwardNoMgtBackward_MuxExtSet();
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
    leave_MgtForwardMgtBackward_MuxExtSet();
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    leave_MgtForwardNoMgtBackward_MuxExtSet();
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : function leaveMuxExtSet : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    leave_NoMgtForwardNoMgtBackward_MuxExtSet();
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
      status = rootMuxExtSet(forward, backward);
      break;
    case FANIN_FANOUT :
      status = faninfanoutMuxExtSet(forward, backward);
      break;
    case LEAVE :
      status = leaveMuxExtSet(forward, backward);
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function muxExtSet\n");
      status = XST_FAILURE;
      break;
  }

  return status;
}

#undef DBG
