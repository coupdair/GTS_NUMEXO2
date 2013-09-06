#include "xdebug_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"

#define DBG XMMR_CLOCKPATH_DBG

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

static int set_user_logic_clock(XMMRegs_ClockPath_ctrl *c, unsigned char choice_clk)
{
  int status = XST_SUCCESS;

  switch (choice_clk) {
    case LOCAL_CLK :
      c->clk_select_usr    = 0;
      c->clk_select_remloc = 0;
      break;
    case RECOVERED_CLK :
      c->clk_select_usr    = 0;
      c->clk_select_remloc = 1;
      break;
    case MICTOR_CLK :
      c->clk_select_usr    = 1;
      c->clk_select_remloc = 0;
      break;
    default :
      DBG(DBLE, "unallowed value of choice in function set_user_logic_clock\n");
      status = XST_FAILURE;
      break; 
  }

  return status;
}

/* one chooses here the clock sent to PLL
it is either the LOCAL clock, or the recovered clock coming from the MASTER transceiver */
static int set_raw_clock(XMMRegs_ClockPath_ctrl *c, unsigned char choice_clk)
{
  int status = XST_SUCCESS;

  switch (choice_clk) {
    case LOCAL_CLK :
      c->clk_select_raw    = 0;
      break;
    case MASTER_CLK :
      c->clk_select_raw    = 1;
      break;
    default :
      DBG(DBLE, "ERROR : unallowed value of choice in function set_raw_clock\n");
      status = XST_FAILURE;
      break; 
  }

  return status;
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

int XMMRegs_ClockPath_UserLogicClock_Set(XMMRegs *InstancePtr, unsigned char choice_clk)
{
  /* local variables */
  int status = XST_SUCCESS;
  XMMRegs_ClockPath_ctrl *c;  

  c = (XMMRegs_ClockPath_ctrl *)(InstancePtr->BaseAddress +  XMMR_CLOCKPATH_CTRL_OFFSET);

  DBG(DBLD, "User logic clock source --->");

  status |= set_user_logic_clock(c, choice_clk); 

  DBG(DBLD, "---> set |\n");

  return status;
}

int XMMRegs_ClockPath_RawClock_Set(XMMRegs *InstancePtr, unsigned char choice_clk)
{
  /* local variables */
  int status = XST_SUCCESS;
  XMMRegs_ClockPath_ctrl *c;  

  c = (XMMRegs_ClockPath_ctrl *)(InstancePtr->BaseAddress +  XMMR_CLOCKPATH_CTRL_OFFSET);

  DBG(DBLD, "Raw clock --->");

  status |= set_raw_clock(c, choice_clk); 

  if (status != XST_SUCCESS) {
    DBG(DBLD, "---> not set |\n");
  }
  else {
    DBG(DBLD, "---> set |\n");
  }

  return status;
}

int XMMRegs_ClockPath_RawClock_Diagnose(XMMRegs *InstancePtr)
{
  /* local variables */
  XMMRegs_ClockPath_ctrl *c;  

  c = (XMMRegs_ClockPath_ctrl *)(InstancePtr->BaseAddress +  XMMR_CLOCKPATH_CTRL_OFFSET);
  
  if (c->clk_select_raw == 0)
    return LOCAL_CLK;
  else
    return MASTER_CLK;
}

int XMMRegs_ClockPath_IsMictorDcmLocked(XMMRegs *InstancePtr)
{
  /* local variables */
  XMMRegs_ClockPath_status *s;  

  s = (XMMRegs_ClockPath_status *)(InstancePtr->BaseAddress +  XMMR_CLOCKPATH_STATUS_OFFSET);

  if (s->dcm_locked_user == 1)
    return DCM_LOCKED;
  else
    return DCM_NOT_LOCKED;
}

int XMMRegs_ClockPath_IsFbDcmLocked(XMMRegs *InstancePtr)
{
  /* local variables */
  XMMRegs_ClockPath_status *s;  

  s = (XMMRegs_ClockPath_status *)(InstancePtr->BaseAddress +  XMMR_CLOCKPATH_STATUS_OFFSET);

  if (s->dcm_locked_fb == 1) 
    return DCM_LOCKED;
  else
    return DCM_NOT_LOCKED;
}

/* this function resets and sets the mictor DCM
be careful, you may lose the phase alignment with this function
if the alignment is done including the path through the carrier 
*/
int XMMRegs_ClockPath_MictorDcm_Launch(XMMRegs *InstancePtr)
{
  XMMRegs_ClockPath_ctrl *c;
  
  c = (XMMRegs_ClockPath_ctrl *) (InstancePtr->BaseAddress + XMMR_CLOCKPATH_CTRL_OFFSET);
  
  c->rst_dcm_mictor = 1;
  c->rst_dcm_mictor = 0;
  
  return XST_SUCCESS;
}

/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

int XMMRegs_ClockPath_Setup(XMMRegs *InstancePtr, int setup_mode)
{
  int status = XST_SUCCESS;
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_ClockPath_ctrl *c;  

  c = (XMMRegs_ClockPath_ctrl *)(ba +  XMMR_CLOCKPATH_CTRL_OFFSET);

  if (setup_mode == XMMR_SETUP) {
    status |= set_user_logic_clock(c, LOCAL_CLK); /* the user logic clock comes from the local reference clock */
    status |= set_raw_clock(c, LOCAL_CLK); /* the raw clock sent to LMK_PLL is the local clock */
  }
  else if (setup_mode == XMMR_UPDATE) {
    if ( ( XMMRegs_IsTransceiverConnected(InstancePtr, MASTER_TRANSCEIVER) == TRANSCEIVER_IS_CONNECTED   ) && 
         ( XMMRegs_IsTransceiverInGtsTree(InstancePtr, MASTER_TRANSCEIVER) == TRANSCEIVER_IS_IN_GTS_TREE ) ) {
    /* the GTS is either LEAVE or FANIN-FANOUT */
      status |= set_user_logic_clock(c, RECOVERED_CLK); /* the user logic clock is recovered */
      status |= set_raw_clock(c, MASTER_CLK); /* the raw clock sent to LMK_PLL is the master clock */
    }
    else {
      status |= set_user_logic_clock(c, LOCAL_CLK); /* the user logic clock comes from the local reference clock */
      status |= set_raw_clock(c, LOCAL_CLK); /* the raw clock sent to LMK_PLL is the local clock */
    }
  }
  else {
    DBG(DBLD, "ERROR : out of range on setup_mode in XMMRegs_ClockPath_Setup\n");
    return XST_FAILURE;
  }
  
  return status;
}

int XMMRegs_ClockPath_Start(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  
  return status;
}

int XMMRegs_ClockPath_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_ClockPath_ctrl *c;  

  c = (XMMRegs_ClockPath_ctrl *)(ba +  XMMR_CLOCKPATH_CTRL_OFFSET);
  
  status |= set_user_logic_clock(c, LOCAL_CLK); /* the user logic clock comes from the local reference clock */
  status |= set_raw_clock(c, LOCAL_CLK); 
  
  return status;
}

int XMMRegs_ClockPath_Init(XMMRegs *InstancePtr)
{
  /* local variables */
  int status = XST_SUCCESS;
  
  DBG(DBLD, "ClockPath_Init --->\n");
  status |= XMMRegs_ClockPath_Setup(InstancePtr, XMMR_SETUP);
  status |= XMMRegs_ClockPath_Start(InstancePtr);
  DBG(DBLD, "---> ClockPath_Init |\n");
  
  return status;
}

int XMMRegs_ClockPath_Reset(XMMRegs *InstancePtr)
{
  /* local variables */
  int status = XST_SUCCESS;

  status |= XMMRegs_ClockPath_Stop(InstancePtr);
  status |= XMMRegs_ClockPath_Init(InstancePtr);
  
  return status;
}

void XMMRegs_ClockPath_PrintAll(XMMRegs *InstancePtr)
{
  DBG(DBLI, "\nPrintAll of ClockPath :--------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "ClockPath_ctrl\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_CLOCKPATH_CTRL_OFFSET);
  DBG(DBLI, "ClockPath_status\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_CLOCKPATH_STATUS_OFFSET);
  
  return;
}

#undef DBG
