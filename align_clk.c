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

/* If at least one transceiver belonging to the same tile is in the GTS tree, 
then refclk will be set to fulfill the alignment procedure */
int doesAlignClkApplyToTile(int tile)
{
  int bool = TRANSCEIVER_IS_NOT_IN_GTS_TREE;
  int trans; 

  for (trans = 2*tile; trans <= 2*tile+1; trans++) {
    bool |= XMMRegs_IsTransceiverInGtsTree(&XMMRegsDriver, trans);
  }

  return bool;
}

int gtsReadySet(void)
{
  return XMMRegs_Trigger_GtsReady_Set(&XMMRegsDriver);
}

int gtsReadyUnSet(void)
{
  return XMMRegs_Trigger_GtsReady_UnSet(&XMMRegsDriver);
}

int isPllLocked(void)
{
  return XMMRegs_MuxInOut_IsPllLocked(&XMMRegsDriver);
}   

int isDcmLocked(int usrclk)
{
  if (usrclk == RECOVERED_CLK)
    return XMMRegs_ClockPath_IsFbDcmLocked(&XMMRegsDriver);
  else if (usrclk == MICTOR_CLK)
    return XMMRegs_ClockPath_IsMictorDcmLocked(&XMMRegsDriver);
  else if (usrclk == LOCAL_CLK) 
    return DCM_LOCKED;
  else {
    DBG(DBLE, "ERROR : out of range for usrclk in isDcmLocked\n");
    return DCM_NOT_LOCKED;
  }
}   

int usrClkLocalSet(void)
{
  return XMMRegs_ClockPath_UserLogicClock_Set(&XMMRegsDriver, LOCAL_CLK);
}

int usrClkRecoveredSet(void)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_ClockPath_UserLogicClock_Set(&XMMRegsDriver, RECOVERED_CLK);

//  while ( isDcmLocked(RECOVERED_CLK) != DCM_LOCKED )
//      loopAction("WAIT : for fb DCM to lock");

  return status;
}

int usrClkMictorSet(void)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_ClockPath_UserLogicClock_Set(&XMMRegsDriver, MICTOR_CLK);

//  while ( isDcmLocked(MICTOR_CLK) != DCM_LOCKED )
//      loopAction("WAIT : for mictor DCM to lock");

  return status;
}

/* looks if the LMK is locked and if the DCM taking the carrier card is locked */
int isCarrierClkReady(int usrclk)
{
  if ( ( alignModeRead() == CARRIER_INCLUDED ) && (usrclk == MICTOR_CLK) ) {
    return XMMRegs_Trigger_CarrierLmkPllLock_Read(&XMMRegsDriver);
  }
  else if ( ( alignModeRead() == CARRIER_INCLUDED ) && (usrclk != MICTOR_CLK) ) {
    DBG(DBLW, "WARNING : the carrier is included, but you don't use MICTOR_CLK in the leaf\n");
    return CLK_CARRIER_READY;
  }
  else if ( ( alignModeRead() != CARRIER_INCLUDED ) && (usrclk != MICTOR_CLK) ) {
    return CLK_CARRIER_READY;
  }
  else if ( ( alignModeRead() != CARRIER_INCLUDED ) && (usrclk == MICTOR_CLK) ) {
    DBG(DBLW, "WARNING (isCarrierClkReady): the LLP carrier is not included\n");
    DBG(DBLW, "Ignore this warning if, on VME Carrier, you have correctly made the clock loopback through SMA cables\n");
    DBG(DBLW, "If the carrier is a LLP Carrier, before using this function, you have to launch  : includeCarrierForAlign()\n"); 
    return CLK_CARRIER_READY; /* there is no check of the presence of the SMA cables */
  }
  else {
    DBG(DBLE, "STRANGE : in isCarrierClkReady\n");
    return CLK_CARRIER_READY;
  }
}   

/* looks if the LMK is locked and if the DCM taking the carrier card is locked */
int isCarrierClkNotReady(int usrclk)
{
  if ( ( alignModeRead() == CARRIER_INCLUDED ) && (usrclk == LOCAL_CLK) ) {
    return XMMRegs_Trigger_CarrierLmkPllLock_Read(&XMMRegsDriver);
  }
  else if ( ( alignModeRead() != CARRIER_INCLUDED ) && (usrclk == LOCAL_CLK) ) {
    return CLK_CARRIER_NOT_READY;
  }
  else {
    DBG(DBLW, "WARNING : in isCarrierClkNotReady : I would expect usrclk = LOCAL_CLK\n");
    return CLK_CARRIER_NOT_READY;
  }
}   

int refClkDiagnose(void) {
  return 1; // NUMEXO2
}

void txSystemReset(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) ) XMMRegs_RocketIO_TxSystem_Reset(&XMMRegsDriver, transceiver);
}

void rxSystemReset(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) ) XMMRegs_RocketIO_RxSystem_Reset(&XMMRegsDriver, transceiver); 
}

void txSystemInit(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) ) XMMRegs_RocketIO_TxSystem_Init(&XMMRegsDriver, transceiver); 
}

void rxSystemInit(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) ) XMMRegs_RocketIO_RxSystem_Init(&XMMRegsDriver, transceiver); 
}

int txSystemStatus(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_TxSystem_Status(&XMMRegsDriver, transceiver);
  else
    return READY; 
}

int rxSystemStatus(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_RxSystem_Status(&XMMRegsDriver, transceiver);
  else
    return READY; 
}

void txSystemResetAll(void) {
  if ( doesAlignClkApplyToTransceiver(0) ) XMMRegs_RocketIO_TxSystem_Reset(&XMMRegsDriver, 0); 
}

void rxSystemResetAll(void) {
  if ( doesAlignClkApplyToTransceiver(0) ) XMMRegs_RocketIO_RxSystem_Reset(&XMMRegsDriver, 0); 
}

void txSystemInitAll(void) {
  if ( doesAlignClkApplyToTransceiver(0) ) XMMRegs_RocketIO_TxSystem_Init(&XMMRegsDriver, 0); 
}

void rxSystemInitAll(void) {
  if ( doesAlignClkApplyToTransceiver(0) ) XMMRegs_RocketIO_RxSystem_Init(&XMMRegsDriver, 0); 
}

int txSystemStatusAll(void) {
  int status = READY;

  if ( doesAlignClkApplyToTransceiver(0) ) {
    if ( XMMRegs_RocketIO_TxSystem_Status(&XMMRegsDriver, 0) != READY ) status = NOT_READY;
  }

  return status; 
}

int rxSystemStatusAll(void) {
  int status = READY;

  if ( doesAlignClkApplyToTransceiver(0) ) {
    if ( XMMRegs_RocketIO_RxSystem_Status(&XMMRegsDriver, 0) != READY ) status = NOT_READY;
  }

  return status; 
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

/********************  leave GTS   *************************/

int leave_MgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "leave_MgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  rxSystemInit(MASTER_TRANSCEIVER);

  txSystemReset(MASTER_TRANSCEIVER);

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        status |= gtsReadySet();
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

  txSystemInitAll();
  rxSystemInitAll();

  DBG(DBLD, "leave_MgtForwardMgtBackward_ClkSet done\n" );

  return status;
}

int leave_NoMgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "leave_NoMgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  if ( alignModeRead() == CARRIER_INCLUDED ) {
    /* one says to the carrier that the clock sent is going to be unlocked */
    status |= gtsReadyUnSet();
  }  

  rxSystemInit(MASTER_TRANSCEIVER);

  txSystemReset(MASTER_TRANSCEIVER);

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        status |= gtsReadySet();
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

  txSystemInitAll();
  rxSystemInitAll();

  DBG(DBLD, "leave_NoMgtForwardMgtBackward_ClkSet done\n" );

  return status;
}

int leave_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;  

  DBG(DBLD, "leave_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  rxSystemInit(MASTER_TRANSCEIVER);

  txSystemResetAll(); /* reset of all active TX PMAs */

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feed the LMK_PLL with the clock recovered from the master transceiver */
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        status |= gtsReadySet();
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
    status |= gtsReadyUnSet();
  }  

  txSystemResetAll(); /* reset of all active TX PMAs */
  rxSystemResetAll(); /* reset of all active RX PMAs */

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

  rxSystemInit(MASTER_TRANSCEIVER);

  txSystemReset(MASTER_TRANSCEIVER);

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

  txSystemInitAll();
  rxSystemInitAll();

  DBG(DBLD, "faninfanout_MgtForwardMgtBackward_ClkSet done\n");

  return status;
}

int faninfanout_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "faninfanout_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  rxSystemInit(MASTER_TRANSCEIVER);

  txSystemResetAll(); /* reset of all active TX PMAs */

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

  txSystemResetAll(); /* reset of all active TX PMAs */
  rxSystemResetAll(); /* reset of all active RX PMAs */

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

  /* We don't reset the tx rocketIOs, indeed if this function is used by mistake,
  a reset would destroy the clocking of the all GTS tree */

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    rawClkSet(LOCAL_CLK);
    txSystemResetAll(); 

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

  txSystemInitAll(); 
  rxSystemResetAll(); 
  rxSystemInitAll(); 

  DBG(DBLD, "root_MgtForwardMgtBackward_ClkSet done\n");

  return status;
}

int root_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  txSystemResetAll(); /* reset of all active TX PMAs */
  rxSystemResetAll(); 

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

  txSystemInitAll(); /* note that if the system has not been previously reset because rawClkDiagnose() == LOCAL_CLK (see 10 rigs above), this will not reset the tx system */

  DBG(DBLD, "root_MgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int root_NoMgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) 
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_NoMgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  txSystemResetAll(); /* reset of all active TX PMAs */
  rxSystemResetAll(); /* reset of all active RX PMAs */

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
