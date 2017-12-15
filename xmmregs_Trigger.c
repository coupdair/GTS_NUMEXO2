#include "xdebug_l.h"
#include "xdiagnose_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xmmregs_Trigger.h"

#define DBG XMMR_TRIGGER_DBG


/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

int XMMRegs_Trigger_PeriodMask_Set(XMMRegs *InstancePtr, int log2_period) 
{
  int status = XST_SUCCESS;
  int i;
  XMMRegs_period_ctrl *c;
  unsigned int *ba;

  if ( (log2_period <= 1) || (log2_period > 32) ) {
    DBG(DBLE, "ERROR : out of range on log2_period in function XMMRegs_Trigger_PeriodMask_Set\n");
    DBG(DBLE, "should be : 1 < log2_period <= 32\n");
    return XST_FAILURE; 
  }

  ba = (unsigned int *)(InstancePtr->BaseAddress + XMMR_PERIOD_CTRL_OFFSET);
  c = (XMMRegs_period_ctrl *)(InstancePtr->BaseAddress + XMMR_PERIOD_CTRL_OFFSET);

  DBG(DBLI, "The sync period mask was\t\t\t:\t");
  print_binary(ba);

  for (i = 0; i < log2_period; i++) {
    c->period_mask = set_bit_to_1(c->period_mask, i);
  }
  for (i = log2_period; i < 32; i++) {
    c->period_mask = set_bit_to_0(c->period_mask, i);
  }

  DBG(DBLI, "The sync period mask has been changed to\t:\t");
  print_binary(ba);

  DBG(DBLI, "It corresponds to\t\t\t\t:\t%f ms between sync signals\n", ((double) (*ba+1.0)) / 100000.0 );

  return status;
}

int XMMRegs_Trigger_CoreType_Get(XMMRegs *InstancePtr) 
{
  XMMRegs_trigger_status *trg_status;

  trg_status = (XMMRegs_trigger_status *)(InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);

  return trg_status->trigger_type;
}

int XMMRegs_Trigger_CoreStop(XMMRegs *InstancePtr) 
{
  XMMRegs_trigger_ctrl *trg_ctrl;

  trg_ctrl = (XMMRegs_trigger_ctrl *)(InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  
  trg_ctrl->rst_core_uP = 1;

  return XST_SUCCESS;
}

int XMMRegs_Trigger_CoreStart(XMMRegs *InstancePtr) 
{
  XMMRegs_trigger_ctrl *trg_ctrl;

  trg_ctrl = (XMMRegs_trigger_ctrl *)(InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  
  trg_ctrl->rst_core_uP = 0;

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Test_Set(XMMRegs *InstancePtr) 
{
  XMMRegs_trigger_ctrl *trg_ctrl;

  trg_ctrl = (XMMRegs_trigger_ctrl *)(InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  
  trg_ctrl->test_enable = 1;

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Test_UnSet(XMMRegs *InstancePtr) 
{
  XMMRegs_trigger_ctrl *trg_ctrl;

  trg_ctrl = (XMMRegs_trigger_ctrl *)(InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  
  trg_ctrl->test_enable = 0;

  return XST_SUCCESS;
}

int XMMRegs_Trigger_CrystalID_Set(XMMRegs *InstancePtr, int crystalID)
{
  XMMRegs_i2cgts_output_ctrl *m;

  if ( (crystalID <= 0) || (crystalID > 0xFF) ) {
    DBG(DBLE, "ERROR : out of range on crystalID in function XMMRegs_Trigger_CrystalID_Set\n");
    return XST_FAILURE; 
  }

  m = (XMMRegs_i2cgts_output_ctrl *) (InstancePtr->BaseAddress + XMMR_I2CGTS_OUTPUT_CTRL_OFFSET);
  m->mask_crystalID = 0xFF; /* gives control of crystalID to the uP */
  m->crystalID      = crystalID; 

  DBG(DBLE, "crystalID has been set to %d\n", crystalID);

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Leave_TestLoopback_Set(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;
  XMMRegs_i2cgts_output_ctrl *ci;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);
  ci = (XMMRegs_i2cgts_output_ctrl *) (InstancePtr->BaseAddress + XMMR_I2CGTS_OUTPUT_CTRL_OFFSET);

  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) != TRIGGER_CORE_IS_LEAF ) {
    DBG(DBLE, "ERROR : trigger core should be leaf, you have the wrong bitstream!\n");
    return XST_FAILURE;
  }

  c->rst_core_uP = 1;
  c->gts_ready = 1;
  c->loopback_enable = 1;
  c->test_enable = 1;

  ci->mask_L1A_gate = 1;
  ci->L1A_gate = 1;

  c->rst_core_uP = 0; 

  if ( s->rst == 1 ) {
    DBG(DBLW, "STRANGE : trigger core is still in reset after XMMRegs_Trigger_Leave_TestLoopback_Set\n");
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}

void XMMRegs_Trigger_GtsReady_Set(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  c->gts_ready = 1;
}

void XMMRegs_Trigger_GtsReady_UnSet(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  c->gts_ready = 0;
}

int XMMRegs_Trigger_Leave_Test_Set(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;
  XMMRegs_i2cgts_output_ctrl *ci;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);
  ci = (XMMRegs_i2cgts_output_ctrl *) (InstancePtr->BaseAddress + XMMR_I2CGTS_OUTPUT_CTRL_OFFSET);

  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) != TRIGGER_CORE_IS_LEAF ) {
    DBG(DBLE, "ERROR : trigger core should be leaf, you have the wrong bitstream!\n");
    return XST_FAILURE;
  }

  c->rst_core_uP = 1;
  c->gts_ready = 1;
  c->loopback_enable = 0;
  c->test_enable = 1;

  ci->mask_L1A_gate = 1;
  ci->L1A_gate = 1;

  c->rst_core_uP = 0; 

  if ( s->rst == 1 ) {
    DBG(DBLW, "STRANGE : trigger core is still in reset after XMMRegs_Trigger_Leave_Test_Set\n");
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Leave_CarrierRequest_Set(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;
  XMMRegs_i2cgts_output_ctrl *ci;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);
  ci = (XMMRegs_i2cgts_output_ctrl *) (InstancePtr->BaseAddress + XMMR_I2CGTS_OUTPUT_CTRL_OFFSET);

  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) != TRIGGER_CORE_IS_LEAF ) {
    DBG(DBLE, "ERROR : trigger core should be leaf, you have the wrong bitstream!\n");
    return XST_FAILURE;
  }

  c->rst_core_uP = 1;
  c->loopback_enable = 0; // pour GTS_LEAF sur NUMEXO2, F. Saillant le 13 septembre 2013
//  c->loopback_enable = 1; // pour GTS_LEAF sur ML507, F. Saillant le 4 septembre 2012
  c->test_enable = 0;

  ci->mask_L1A_gate = 1;
  ci->L1A_gate = 1;

  c->rst_core_uP = 0; 

  if ( s->rst == 1 ) {
    DBG(DBLW, "STRANGE : trigger core is still in reset after XMMRegs_Trigger_Leave_CarrierRequest_Set\n");
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Root_Loopback_Set(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;
  
  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);
  
  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) != TRIGGER_CORE_IS_ROOT ) {
    DBG(DBLE, "ERROR : trigger core should be root, you have the wrong bitstream!\n");
    return XST_FAILURE;
  }

  c->rst_core_uP = 1;
  c->gts_ready = 1;
  c->aurora_loopback = 0;
  c->loopback_enable = 1; /* the trigger core will validate all events */

  c->rst_core_uP = 0; 

  if ( s->rst == 1 ) {
    DBG(DBLW, "STRANGE : trigger core is still in reset after XMMRegs_Trigger_Root_Loopback_Set\n");
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Daq_Start(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);

  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) != TRIGGER_CORE_IS_ROOT ) {
    DBG(DBLE, "ERROR : trigger core should be root, you have the wrong bitstream!\n");
    return XST_FAILURE;
  }

  if ( (s->rst == 1) || (c->rst_core_uP == 1) || (c->gts_ready == 0) || (c->aurora_loopback == 1) ) {
    DBG(DBLE, "ERROR : did you launch XMMRegs_Trigger_Root_Loopback_Set beforehand ?");
    return XST_FAILURE;
  }

  if (c->loopback_enable == 1) {
    DBG(DBLW, "WARNING : the root node is already validating all events\n"); 
  }

  c->loopback_enable = 1; /* the trigger core will now validate all events */

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Daq_Stop(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;
  
  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);
  
  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) != TRIGGER_CORE_IS_ROOT ) {
    DBG(DBLE, "ERROR : trigger core should be root, you have the wrong bitstream!\n");
    return XST_FAILURE;
  }
  
  if ( (s->rst == 1) || (c->rst_core_uP == 1) || (c->gts_ready == 0) || (c->aurora_loopback == 1) ) {
    DBG(DBLE, "ERROR : did you launch XMMRegs_Trigger_Root_Loopback_Set beforehand ?");
    return XST_FAILURE;
  }

  if (c->loopback_enable == 0) {
    DBG(DBLW, "WARNING : the root node is already rejecting all events\n"); 
  }

  c->loopback_enable = 0; /* the trigger core will now reject all events */

  return XST_SUCCESS;
}

int XMMRegs_Trigger_Root_AuroraLoopback_Set(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);

  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) != TRIGGER_CORE_IS_ROOT ) {
    DBG(DBLE, "ERROR : trigger core should be root, you have the wrong bitstream!\n");
    return XST_FAILURE;
  }

  c->rst_core_uP = 1;
  c->gts_ready = 1;
  c->aurora_loopback = 1;
  c->loopback_enable = 0;

  c->rst_core_uP = 0; 

  if ( s->rst == 1 ) {
    DBG(DBLW, "STRANGE : trigger core is still in reset after XMMRegs_Trigger_Leave_Test_Set\n");
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}

/* this function resets and sets the user DCM
be careful, you may lose the phase alignment with this function
if the alignment is done including the path through the carrier 
*/
int XMMRegs_Trigger_UserDcm_Launch(XMMRegs *InstancePtr)
{
  XMMRegs_trigger_ctrl *c;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);

  c->rst_dcm_uP = 1;
  c->rst_dcm_uP = 0;

  return XST_SUCCESS;
}

int XMMRegs_Trigger_CarrierLmkPllLock_Read(XMMRegs *InstancePtr)
{
  XMMRegs_carrier_status *s;

  s = (XMMRegs_carrier_status *) (InstancePtr->BaseAddress + XMMR_CARRIER_STATUS_OFFSET);

  if (s->LMK_PLL_locked == 1) {
    return CLK_CARRIER_READY; 
  }
  else {
    return CLK_CARRIER_NOT_READY;
  }
}

int XMMRegs_Trigger_CarrierType_Read(XMMRegs *InstancePtr)
{
  XMMRegs_carrier_status *s;

  s = (XMMRegs_carrier_status *) (InstancePtr->BaseAddress + XMMR_CARRIER_STATUS_OFFSET);

  if (s->LLP_Carrier_is_here == 1) {
    return LLP_CARRIER_IS_CARRIER; 
  }
  else {
    return VME_CARRIER_IS_CARRIER;
  }
}

/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

int  XMMRegs_Trigger_Setup(XMMRegs *InstancePtr)
{
  /* local variables */
  XMMRegs_trigger_ctrl *c;
  XMMRegs_trigger_status *s;
  XMMRegs_i2cgts_output_ctrl *ci;
  int status = XST_SUCCESS;
  unsigned int card_number;

  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  s = (XMMRegs_trigger_status *) (InstancePtr->BaseAddress + XMMR_TRIGGER_STATUS_OFFSET);
  ci = (XMMRegs_i2cgts_output_ctrl *) (InstancePtr->BaseAddress + XMMR_I2CGTS_OUTPUT_CTRL_OFFSET);

  c->timestamp_enable = 1;
  c->aurora_loopback = 0;
  c->test_enable = 0;
  c->loopback_enable = 1;
  c->gts_ready = 0; /* it says to the carrier that the GTS alignment is ready */
  c->rst_core_uP = 1; /* it maintains the trigger core in reset */
  c->rst_dcm_uP = 0;

  ci->mask_L1A_gate = 0;
  ci->L1A_gate = 0;
  ci->mask_LMK_PLL_locked = 0;
  ci->LMK_PLL_locked = 0;

  /* one sets the crystalID to the card number, at first */
  if ( XMMRegs_Trigger_CoreType_Get(InstancePtr) == TRIGGER_CORE_IS_LEAF ) {
    card_number = *((unsigned int *)CARD_NUMBER_ADDRESS);
    status |= XMMRegs_Trigger_CrystalID_Set(InstancePtr, (int) card_number);
  }

//  status |= XMMRegs_Trigger_PeriodMask_Set(InstancePtr, 16);

  return status;
}

int  XMMRegs_Trigger_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  return status;
}

int  XMMRegs_Trigger_Start(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  
  return status;
}

int  XMMRegs_Trigger_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "Trigger_Init --->\n");
  status |= XMMRegs_Trigger_Setup(InstancePtr);
  status |= XMMRegs_Trigger_Start(InstancePtr);
  DBG(DBLD, "---> Trigger_Init |\n");

  return status;
}

int  XMMRegs_Trigger_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_Trigger_Stop(InstancePtr);
  status |= XMMRegs_Trigger_Init(InstancePtr);

  return status;
}

void XMMRegs_Trigger_PrintAll(XMMRegs *InstancePtr)
{  
  int i;

  DBG(DBLI, "\nPrintAll of Trigger :---------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "trigger_ctrl\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_TRIGGER_CTRL_OFFSET); 
//  DBG(DBLI, "i2cgts_input_ctrl\t:\t"); 
//  XMMRegs_PrintBinary(InstancePtr, XMMR_I2CGTS_INPUT_CTRL_OFFSET); 
  DBG(DBLI, "i2cgts_output_ctrl\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_I2CGTS_OUTPUT_CTRL_OFFSET); 
//  DBG(DBLI, "trigger_conf_ctrl\t:\t"); 
//  XMMRegs_PrintBinary(InstancePtr, XMMR_TRIGGER_CONF_CTRL_OFFSET); 
//  DBG(DBLI, "period_ctrl\t\t:\t"); 
//  XMMRegs_PrintBinary(InstancePtr, XMMR_PERIOD_CTRL_OFFSET); 
  DBG(DBLI, "carrier_status\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_CARRIER_STATUS_OFFSET); 
  DBG(DBLI, "trigger_status\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_TRIGGER_STATUS_OFFSET); 
  DBG(DBLI, "trigger_internal_msw_status :\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_TRIGGER_INTERNAL_STATUS_OFFSET); 
  DBG(DBLI, "trigger_internal_lsw_status :\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_TRIGGER_INTERNAL_STATUS_OFFSET+0x4); 
  for (i = 0; i <= 0; i++) {
    DBG(DBLI, "upstream_leaf(%d, %d)\t:\t", 2*i+1, 2*i); 
    XMMRegs_PrintHex(InstancePtr, XMMR_UPSTREAM_LEAF_STATUS_OFFSET + 4*i);
  }
  for (i = 0; i <= 0; i++) {
    DBG(DBLI, "downstream_leaf(%d, %d)\t:\t", 2*i+1, 2*i); 
    XMMRegs_PrintHex(InstancePtr, XMMR_DOWNSTREAM_LEAF_STATUS_OFFSET + 4*i);
  }
/*
  for (i = 1; i >= 0; i--) {
    DBG(DBLI, "upstream_root(%d->%d)\t:\t", 32*i+31, 32*i); 
    XMMRegs_PrintHex(InstancePtr, XMMR_UPSTREAM_ROOT_STATUS_OFFSET + 4*i);
  }
  for (i = 1; i >= 0; i--) {
    DBG(DBLI, "downstream_root(%d->%d)\t:\t", 32*i+31, 32*i); 
    XMMRegs_PrintHex(InstancePtr, XMMR_DOWNSTREAM_ROOT_STATUS_OFFSET + 4*i);
  }
*/
  return;
}

void XMMRegs_Trigger_Diagnose(XMMRegs *InstancePtr)
{  
  XMMRegs_trigger_ctrl *c;

  DBG(DBLI, "\nDiagnose of Trigger :---------------------------------------------------------------\n");
  c = (XMMRegs_trigger_ctrl *) (InstancePtr->BaseAddress + XMMR_TRIGGER_CTRL_OFFSET);
  DIAGN_ONE("timestamp_enable", c->timestamp_enable, 1, "put timestamp_enable to 1");
  DIAGN_WARN("aurora_loopback", c->aurora_loopback, 0, "aurora is in loopback mode; it doesn't expect a trigger processor");
  DIAGN_WARN("test_enable", c->test_enable, 0, "the GTS generates its own trigger requests");
  DIAGN_WARN("loopback_enable", c->loopback_enable, 0, "the GTS generates its own trigger requests");
  DIAGN_ONE("rst_core_uP", c->rst_core_uP, 0, "trigger core is in reset");
  DIAGN_ONE("rst_dcm_uP",  c->rst_dcm_uP,  0, "reset for user_dcm is set to 1");
  DIAGN_ONE("gts_ready",   c->gts_ready,   1, "gts_ready set to 0");

  return;
}

#undef DBG
