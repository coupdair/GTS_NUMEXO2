#include <time.h>
#include "align.h"
#include "xmmregs_i.h"
#include "debug.h"

#define TRUE 1
#define FALSE 0

#define DBG DBG_ALIGN_CLK

/************************************************************

		LOW LEVEL FUNCTIONS

************************************************************/

static unsigned int _time_init_loop_;

static int loopAction(char *s)
{
  unsigned int current_time, diff;
  current_time = takeTime();
  diff = diffTime(_time_init_loop_, current_time); /* given in ns */
  if (diff > 1000*DELAY_IN_US_BETWEEN_LOG_IN_LOOP) {
    DBG(DBLI, "%s\n", s);
  }
  _time_init_loop_ = current_time;
  return XST_SUCCESS;
}

int testTime(unsigned int delay_in_us, unsigned int nb_loop)
{
  unsigned int t1,t2,diff;
  int i;

  t1 = takeTime();
  if (nb_loop > 0) {
    for (i = 0; i < nb_loop; i++) {
      microsleep(delay_in_us); 
    }
  }
  t2 = takeTime();
  diff = diffTime(t1,t2);
  DBG(DBLI, "diff = %u us\n", diff/1000);
  return XST_SUCCESS;
}

/* refclk and usrclk are set depending if the transceiver is or not in the GTS tree.
Set the Status unsigned int vector in the XMMRegsDriver structure accordingly */
int doesAlignClkApplyToTransceiver(int transceiver)
{
  int bool = TRANSCEIVER_IS_IN_GTS_TREE;

  bool &= XMMRegs_IsTransceiverInGtsTree(&XMMRegsDriver, transceiver);

  return bool;
}

void gtsReadySet(void)
{
  XMMRegs_Trigger_GtsReady_Set(&XMMRegsDriver);
}

void gtsReadyUnSet(void)
{
  XMMRegs_Trigger_GtsReady_UnSet(&XMMRegsDriver);
}

int usrClkLocalSet(void)
{
  return XMMRegs_ClockPath_UserLogicClock_Set(&XMMRegsDriver, LOCAL_CLK);
}

int usrClkRecoveredSet(void)
{
  return XMMRegs_ClockPath_UserLogicClock_Set(&XMMRegsDriver, RECOVERED_CLK);
}

int usrClkMictorSet(void)
{
  return XMMRegs_ClockPath_UserLogicClock_Set(&XMMRegsDriver, MICTOR_CLK);
}

void gtxReset(void) {
  if ( doesAlignClkApplyToTransceiver(0) ) XMMRegs_RocketIO_Gtx_Reset(&XMMRegsDriver);
}

void gtxInit(void) {
  if ( doesAlignClkApplyToTransceiver(0) ) XMMRegs_RocketIO_Gtx_Init(&XMMRegsDriver); 
}

/************************************************************

                  LOW LEVEL FUNCTIONS

************************************************************/

int usrClkSet(int usrclk) {
  int status = XST_SUCCESS;

  switch (usrclk) {
    case LOCAL_CLK :
      status |= usrClkLocalSet();
      break;
    case RECOVERED_CLK :
      status |= usrClkRecoveredSet();
      break;
    case MICTOR_CLK :
      status |= usrClkMictorSet();
      break;
    default :
      DBG(DBLE, "ERROR : not a valid usr clk\n");
      status = XST_FAILURE;
      break;
  }

  return status;
}

/* raw_clk can be either LOCAL_CLK or MASTER_CLK */
int rawClkSet(int raw_clk) {
  return XMMRegs_ClockPath_RawClock_Set(&XMMRegsDriver, raw_clk);
}

/* raw_clk can be either LOCAL_CLK or MASTER_CLK */
int rawClkDiagnose(void) {
  return XMMRegs_ClockPath_RawClock_Diagnose(&XMMRegsDriver);
}

/************************************************************

                  SPECIFIC SET FUNCTIONS

************************************************************/

int leave_MgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "leave_MgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        gtsReadySet();
      }

      status |= usrClkSet(usrclk);
      break;

    case RECOVERED_CLK :
      status |= usrClkSet(usrclk);
      break;

    case LOCAL_CLK :
      DBG(DBLW, "WARNING : you use the local clk as usrclk, but the recovered clock as refclk\n");
      status = XST_FAILURE;
      break;

    default :
      DBG(DBLE, "ERROR : wrong value of usrclk=%d", usrclk);
      return XST_FAILURE;
  }

  DBG(DBLD, "leave_MgtForwardMgtBackward_ClkSet done\n" );

  return status;
}

int leave_NoMgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "leave_NoMgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( alignModeRead() == CARRIER_INCLUDED ) {
    /* one says to the carrier that the clock sent is going to be unlocked */
    gtsReadyUnSet();
  }  

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        gtsReadySet();
      }

      status |= usrClkSet(usrclk);
      break;

    case RECOVERED_CLK :
      status |= usrClkSet(usrclk);
      break;

    case LOCAL_CLK :
      DBG(DBLW, "WARNING : you use the local clk as usrclk, but the recovered clock as refclk\n");
      status = XST_FAILURE;
      break;

    default :
      DBG(DBLE, "ERROR : wrong value of usrclk=%d", usrclk);
      return XST_FAILURE;
  }

  DBG(DBLD, "leave_NoMgtForwardMgtBackward_ClkSet done\n" );

  return status;
}

int leave_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;  

  DBG(DBLD, "leave_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feed the LMK_PLL with the clock recovered from the master transceiver */
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        gtsReadySet();
      }

      status |= usrClkSet(usrclk);
      break;

    case RECOVERED_CLK :
      status |= usrClkSet(usrclk);
      break;

    case LOCAL_CLK :
      DBG(DBLW, "WARNING : you use the local clk as usrclk, but the recovered clock as refclk\n");
      DBG(DBLW, "It is not the correct way to initialize the GTS tree\n");
      status = XST_FAILURE;
      break;

    default :
      DBG(DBLE, "ERROR : wrong value of usrclk=%d in leave_MgtForwardNoMgtBackward_ClkSet\n", usrclk);
      return XST_FAILURE;
  }

  DBG(DBLD, "leave_MgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int leave_NoMgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) 
{
  int status = XST_SUCCESS;

  DBG(DBLD, "leave_NoMgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( alignModeRead() == CARRIER_INCLUDED ) {
    /* one says to the carrier that the clock sent is going to be unlocked */
    gtsReadyUnSet();
  }  

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);
  }

  switch (usrclk) {
    case MICTOR_CLK : case RECOVERED_CLK:
      DBG(DBLE, "ERROR : you can't use the recovered (or mictor) clock when the leaf is in BYPASS_MGT/SFP mode\n");
      return XST_FAILURE;
    case LOCAL_CLK :
      status |= usrClkSet(usrclk);
      break;
    default :
      DBG(DBLE, "ERROR : wrong value of usrclk=%d in leave_NoMgtForwardNoMgtBackward_ClkSet", usrclk);
      return XST_FAILURE;
  }

  DBG(DBLD, "leave_NoMgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int faninfanout_MgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "faninfanout_MgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */
  }

  if (usrclk == MICTOR_CLK) {
    DBG(DBLE, "ERROR : you can't use the mictor clock on a faninfanout GTS\n");
    return XST_FAILURE;
  }
  else {
    status |= usrClkSet(usrclk);
    if (usrclk == LOCAL_CLK) {
      DBG(DBLW, "WARNING : you use the local clk as usrclk, but the recovered clock as refclk\n");
      status = XST_FAILURE;
    }
  }

  DBG(DBLD, "faninfanout_MgtForwardMgtBackward_ClkSet done\n");

  return status;
}

int faninfanout_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "faninfanout_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feed the LMK_PLL with the clock recovered from the master transceiver */
  }

  switch (usrclk) {
    case RECOVERED_CLK :
      status |= usrClkSet(usrclk);
      break;
    case LOCAL_CLK :
      DBG(DBLW, "WARNING : you use the local clk as usrclk, but the recovered clock as refclk\n");
      DBG(DBLW, "It is not the correct way to initialize the GTS tree\n");
      return XST_FAILURE;
    case MICTOR_CLK :
      DBG(DBLE, "ERROR : you can't use the mictor clock on a faninfanout GTS\n");
      DBG(DBLE, "clock not set\n");
      return XST_FAILURE;
    default :
      DBG(DBLE, "ERROR : out of range on usrclk in faninfanout_MgtForwardNoMgtBackward_ClkSet\n");
      return XST_FAILURE;
  }

  DBG(DBLD, "faninfanout_MgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int faninfanout_NoMgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) 
{
  int status = XST_SUCCESS;

  DBG(DBLD, "faninfanout_NoMgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);
  }

  switch (usrclk) {
    case MICTOR_CLK :
      DBG(DBLE, "ERROR : you can't use the mictor clock on a faninfanout GTS\n");
      return XST_FAILURE;
    case RECOVERED_CLK : 
      DBG(DBLE, "ERROR : you can't use recovered clocks when the faninfanout is in BYPASS_MGT/SFP mode\n");
      return XST_FAILURE;
    case LOCAL_CLK :
      status |= usrClkSet(usrclk);
      break;
    default :
      DBG(DBLE, "ERROR : out of range on usrclk in faninfanout_NoMgtForwardNoMgtBackward_ClkSet\n");
      return XST_FAILURE;
  }

  DBG(DBLD, "faninfanout_NoMgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int root_MgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_MgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    rawClkSet(LOCAL_CLK);

    if (force_set == DONT_FORCE_SET) {
      DBG(DBLW, "WARNING : forward = USE_MGT and backward = USE_MGT in root node\n");
      DBG(DBLW, "while rawclk != LOCAL_CLK; it is not the correct way to initialize the GTS tree\n");
      status = XST_FAILURE;
    }
  }

  switch (usrclk) {
    case LOCAL_CLK  :
      status |= usrClkSet(usrclk);
      break;
    case RECOVERED_CLK : case MICTOR_CLK :
      DBG(DBLE, "ERROR : you can't use recovered clocks. It is the root node!\n");
      return XST_FAILURE;
    default :
      DBG(DBLE, "ERROR : out of range on usrclk in root_MgtForwardMgtBackward_ClkSet\n");
      return XST_FAILURE;
  }

  DBG(DBLD, "root_MgtForwardMgtBackward_ClkSet done\n");

  return status;
}

int root_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);
  }

  switch (usrclk) {
    case LOCAL_CLK  :
      status |= usrClkSet(usrclk);
      break;
    case RECOVERED_CLK : case MICTOR_CLK :
      DBG(DBLE, "ERROR : you can't use recovered clocks. It is the root node!\n");
      return XST_FAILURE;
    default :
      DBG(DBLE, "ERROR : out of range on usrclk in root_MgtForwardNoMgtBackward_ClkSet\n");
      return XST_FAILURE;
  }

  DBG(DBLD, "root_MgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int root_NoMgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) 
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_NoMgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);
  }

  switch (usrclk) {
    case LOCAL_CLK  :
      status |= usrClkSet(usrclk);
      break;
    case RECOVERED_CLK : case MICTOR_CLK :
      DBG(DBLE, "ERROR : you can't use recovered clocks. It is the root node!\n");
      status = XST_FAILURE;
      break;
    default :
      DBG(DBLE, "ERROR : out of range on usrclk in root_NoMgtForwardNoMgtBackward_ClkSet\n");
      break;
  }

  DBG(DBLD, "root_NoMgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

/************************************************************

		HIGH LEVEL SET FUNCTIONS

************************************************************/

int rootClkSet(int usrclk, int forward, int backward, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  if      ( (forward == USE_MGT) && (backward == USE_MGT) ) {
    status = root_MgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status = root_MgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status = root_NoMgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else {
    DBG(DBLE, "ERROR : out of range error on forward or backward values in function rootClkSet\n");
    status = XST_FAILURE;
  }

  return status;
}

int leaveClkSet(int usrclk, int forward, int backward, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  if      ( (forward == USE_MGT) && (backward == USE_MGT) ) {
    status = leave_MgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status = leave_MgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    status = leave_NoMgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status = leave_NoMgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else {
    DBG(DBLE, "ERROR : out of range error on forward or backward values in function leaveClkSet\n");
    status = XST_FAILURE;
  }

  return status;
}

int faninfanoutClkSet(int usrclk, int forward, int backward, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  if      ( (forward == USE_MGT) && (backward == USE_MGT) ) {
    status = faninfanout_MgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status = faninfanout_MgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status = faninfanout_NoMgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else {
    DBG(DBLE, "ERROR : out of range error on forward or backward values in function faninfanoutClkSet\n");
    status = XST_FAILURE;
  }

  return status;
}

int clkSet(int GTStype, int usrclk, int forward, int backward, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  switch (GTStype) {
    case ROOT :
      status = rootClkSet(usrclk, forward, backward, force_set, block_mode);
      break;
    case FANIN_FANOUT :
      status = faninfanoutClkSet(usrclk, forward, backward, force_set, block_mode);
      break;
    case LEAVE :
      status = leaveClkSet(usrclk, forward, backward, force_set, block_mode);
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function clkSet\n");
      status = XST_FAILURE;
      break; 
  }

  return status;
}

#undef DBG
