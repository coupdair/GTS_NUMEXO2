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

int refClk2Set(void)
{
  int tile, transceiver;
  int status = XST_SUCCESS;

  DBG(DBLD, "Reference clock source --->");

  if ( doesAlignClkApplyToTile(0) ) status |= XMMRegs_RocketIO_RefclkTX_Set(&XMMRegsDriver, 0, REFCLK2);

  if ( doesAlignClkApplyToTransceiver(0) ) status |= XMMRegs_RocketIO_RefclkRX_Set(&XMMRegsDriver, 0, REFCLK2);

  DBG(DBLD, "---> set |\n");

  return status;
}

int handleMictorDcm(void)
{
  int status = XST_SUCCESS;

  if ( XMMRegs_ClockPath_IsMictorDcmLocked(&XMMRegsDriver) == DCM_LOCKED ) {
    DBG(DBLD, "handleMictorDcm : mictor DCM is locked, nothing is done\n"); 
  }
  else {
    DBG(DBLD, "handleMictorDcm : mictor DCM is not locked, launching user_dcm reset\n"); 
    DBG(DBLD, "mictor DCM is...");
    status |= XMMRegs_ClockPath_MictorDcm_Launch(&XMMRegsDriver);

    while ( XMMRegs_ClockPath_IsMictorDcmLocked(&XMMRegsDriver) != DCM_LOCKED ) ;

    DBG(DBLD, "locked\n");
  }

  return status; 
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

/* reports which refclk is set and if it is correctly set 
returns N0_REFCLK if there is a mismatch */
int refClkDiagnose(void) {
  int refclk = NO_REFCLK;
  int refclk_t[2];
  int i;

  refclk_t[0] = XMMRegs_RocketIO_RefClkRX_Diagnose (&XMMRegsDriver, 0);

  refclk_t[1] = XMMRegs_RocketIO_RefClkTX_Diagnose (&XMMRegsDriver, 0);

  refclk = refclk_t[0];

  for (i = 0; i < 2; i++) {
    if (refclk_t[i] == REFCLK2) {
      DBG(DBLD, "REFCLK2 : "); 
    }
    else if (refclk_t[i] == REFCLK1) {
      DBG(DBLD, "REFCLK1 : "); 
    }
    else {
      DBG(DBLD, "NO_REFCLK : ");
    } 
  }

  DBG(DBLD, "refclk of interest --> ");

  if (refclk == REFCLK2) {
    DBG(DBLD, "REFCLK2\n"); 
  }
  else if (refclk == REFCLK1) {
    DBG(DBLD, "REFCLK1\n"); 
  }
  else {
    DBG(DBLD, "NO_REFCLK\n");
  } 

  return refclk;
}

int txSystemReset(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_TxSystem_Reset(&XMMRegsDriver, transceiver);
  else 
    return XST_SUCCESS; 
}

int rxSystemReset(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_RxSystem_Reset(&XMMRegsDriver, transceiver); 
  else 
    return XST_SUCCESS; 
}

int txSystemInit(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_TxSystem_Init(&XMMRegsDriver, transceiver); 
  else 
    return XST_SUCCESS; 
}

int rxSystemInit(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_RxSystem_Init(&XMMRegsDriver, transceiver); 
  else 
    return XST_SUCCESS; 
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

int txPcsReset(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_TxPcs_Reset(&XMMRegsDriver, transceiver);
  else
    return XST_SUCCESS; 
}

int rxPcsReset(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_RxPcs_Reset(&XMMRegsDriver, transceiver);
  else
    return XST_SUCCESS; 
}

int txPcsInit(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_TxPcs_Init(&XMMRegsDriver, transceiver);
  else
    return XST_SUCCESS; 
}

int rxPcsInit(int transceiver) {
  if ( doesAlignClkApplyToTransceiver(transceiver) )
    return XMMRegs_RocketIO_RxPcs_Init(&XMMRegsDriver, transceiver);
  else
    return XST_SUCCESS; 
}

int txSystemResetAll(void) {
  int status = XST_SUCCESS;

  if ( doesAlignClkApplyToTransceiver(0) ) status |= XMMRegs_RocketIO_TxSystem_Reset(&XMMRegsDriver, 0); 

  return status; 
}

int rxSystemResetAll(void) {
  int status = XST_SUCCESS;

  if ( doesAlignClkApplyToTransceiver(0) ) status |= XMMRegs_RocketIO_RxSystem_Reset(&XMMRegsDriver, 0); 

  return status; 
}

int rxSystemResetAllExcept(int trans_excluded) {
  int status = XST_SUCCESS;

  if ( doesAlignClkApplyToTransceiver(0) ) {
    if (trans_excluded != 0) status |= XMMRegs_RocketIO_RxSystem_Reset(&XMMRegsDriver, 0);
  }

  return status; 
}

int txSystemInitAll(void) {
  int status = XST_SUCCESS;

  if ( doesAlignClkApplyToTransceiver(0) ) status |= XMMRegs_RocketIO_TxSystem_Init(&XMMRegsDriver, 0); 

  return status; 
}

int txSystemInitAllExcept(int trans_excluded) {
  int status = XST_SUCCESS;

  if ( doesAlignClkApplyToTransceiver(0) ) {
    if (trans_excluded != 0) status |= XMMRegs_RocketIO_TxSystem_Init(&XMMRegsDriver, 0);
  }

  return status; 
}

int rxSystemInitAll(void) {
  int status = XST_SUCCESS;

  if ( doesAlignClkApplyToTransceiver(0) ) status |= XMMRegs_RocketIO_RxSystem_Init(&XMMRegsDriver, 0); 

  return status; 
}

int txSystemStatusAll(void) {
  int status = READY;

  if ( doesAlignClkApplyToTransceiver(0) ) {
    if ( XMMRegs_RocketIO_TxSystem_Status(&XMMRegsDriver, 0) != READY ) status = NOT_READY;
  }

  return status; 
}

int txSystemStatusAllExcept(int trans_excluded) {
  int status = READY;

  if ( doesAlignClkApplyToTransceiver(0) ) {
    if (trans_excluded != 0) {
      if ( XMMRegs_RocketIO_TxSystem_Status(&XMMRegsDriver, 0) != READY ) status = NOT_READY;
    }
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

  status |= rxSystemInit(MASTER_TRANSCEIVER);

//  while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//    loopAction("WAIT : for gts_pll to lock, and rx(MASTER_TRANS) to be ready"); /* wait for the external PLL to be locked with the current LMK_PLL setting and for the RX master transceiver to be ready */
  while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
    loopAction("WAIT : for rx(MASTER_TRANS) to be ready"); /* wait for the RX master transceiver to be ready */

  status |= txSystemReset(MASTER_TRANSCEIVER);
//  status |= rxSystemResetAllExcept(MASTER_TRANSCEIVER);

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */

//    while ( ( isPllLocked() == PLL_NOT_LOCKED) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for gts_pll to lock, and rx(MASTER_TRANS) to be ready, after the switch of rawclk to MASTER_CLK");

    while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
      loopAction("WAIT : for rx(MASTER_TRANS) to be ready, after the switch of rawclk to MASTER_CLK");
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        status |= gtsReadySet();
      }

//      while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) )
//        loopAction("WAIT : for the READY signal from the carrier before setting usrclk\n");

//      while ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//        loopAction("WAIT : for the READY signal from the carrier before setting usrclk\n");

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

  status |= txSystemInitAll();
  status |= rxSystemInitAll();

  if (raw_clk_changed == TRUE)
  {
    status |= rxPcsReset(MASTER_TRANSCEIVER); /* the RX PCS fifo of the master RocketIO may have become full
    due to mismatches between the local and the recovered clock. This corrects for it. */
    status |= rxPcsInit(MASTER_TRANSCEIVER);
  }

//  if (block_mode == BLOCKING)
//  {
//    while (  ( isPllLocked() == PLL_NOT_LOCKED ) 
//          || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//          || ( isDcmLocked(usrclk) != DCM_LOCKED )
//          || ( txSystemStatusAll() != READY ) 
//          || ( rxSystemStatusAll() != READY ) )
//     loopAction("WAIT : for pll, dcm, all tx and rx, and for carrier to be ready, blocking mode, EOf");
//  }
//  else {
//    while (  ( isPllLocked() == PLL_NOT_LOCKED ) 
//          || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//          || ( isDcmLocked(usrclk) != DCM_LOCKED )
//          || ( txSystemStatusAll() != READY ) 
//          || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for pll, dcm, rx(MASTER_TRANS) and all tx, and for carrier to be ready, EOf");
//  }

//  if (block_mode == BLOCKING)
//  {
//    while ( ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) || ( txSystemStatusAll() != READY ) || ( rxSystemStatusAll() != READY ) )
//     loopAction("WAIT : for all tx and rx, and for carrier to be ready, blocking mode, EOf");
//  }
//  else {
//    while ( ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) || ( txSystemStatusAll() != READY ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for rx(MASTER_TRANS) and all tx, and for carrier to be ready, EOf");
//  }

  if (block_mode == BLOCKING)
  {
    while ( ( txSystemStatusAll() != READY ) || ( rxSystemStatusAll() != READY ) )
     loopAction("WAIT : for all tx and rx to be ready, blocking mode, EOf");
  }
  else {
    while ( ( txSystemStatusAll() != READY ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
      loopAction("WAIT : for rx(MASTER_TRANS) and all tx to be ready, EOf");
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
    status |= gtsReadyUnSet();
  }  

  status |= rxSystemInit(MASTER_TRANSCEIVER);

//  while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//    loopAction("WAIT : for gts_pll to lock, and rx(MASTER_TRANS) to be ready"); /* wait for the external PLL to be locked with the currentLMK_PLL setting and for the RX master transceiver to be ready */

  while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
    loopAction("WAIT : for rx(MASTER_TRANS) to be ready"); /* wait for the RX master transceiver to be ready */

  status |= txSystemReset(MASTER_TRANSCEIVER);
//  status |= rxSystemResetAllExcept(MASTER_TRANSCEIVER);

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */

//    while ( ( isPllLocked() == PLL_NOT_LOCKED) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for gts_pll to lock, and rx(MASTER_TRANS) to be ready, after the switch of rawclk to MASTER_CLK");

    while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
      loopAction("WAIT : for rx(MASTER_TRANS) to be ready, after the switch of rawclk to MASTER_CLK");
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        status |= gtsReadySet();
      }

//      while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) )
//        loopAction("WAIT : for the READY signal from the carrier before setting usrclk\n");

//      while ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//        loopAction("WAIT : for the READY signal from the carrier before setting usrclk\n");

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

  status |= txSystemInitAll();
  status |= rxSystemInitAll();

  if (raw_clk_changed == TRUE)
  {
    status |= rxPcsReset(MASTER_TRANSCEIVER); /* the RX PCS fifo of the master RocketIO may have become full
    due to mismatches between the local and the recovered clock. This corrects for it. */
    status |= rxPcsInit(MASTER_TRANSCEIVER);
  }

//  if (block_mode == BLOCKING)
//  {
//    while (  ( isPllLocked() == PLL_NOT_LOCKED ) 
//          || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//          || ( isDcmLocked(usrclk) != DCM_LOCKED )
//          || ( txSystemStatusAll() != READY ) 
//          || ( rxSystemStatusAll() != READY ) )
//     loopAction("WAIT : for pll, dcm, all tx and rx, and for carrier to be ready, blocking mode, EOf");
//  }
//  else {
//    while (  ( isPllLocked() == PLL_NOT_LOCKED ) 
//          || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//          || ( isDcmLocked(usrclk) != DCM_LOCKED )
//          || ( txSystemStatusAll() != READY ) 
//          || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for pll, dcm, rx(MASTER_TRANS) and all tx, and for carrier to be ready, EOf");
//  }

//  if (block_mode == BLOCKING)
//  {
//    while ( ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) || ( txSystemStatusAll() != READY ) || ( rxSystemStatusAll() != READY ) )
//     loopAction("WAIT : for all tx and rx, and for carrier to be ready, blocking mode, EOf");
//  }
//  else {
//    while ( ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) || ( txSystemStatusAll() != READY ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for rx(MASTER_TRANS) and all tx, and for carrier to be ready, EOf");
//  }

  if (block_mode == BLOCKING)
  {
    while ( ( txSystemStatusAll() != READY ) || ( rxSystemStatusAll() != READY ) )
     loopAction("WAIT : for all tx and rx to be ready, blocking mode, EOf");
  }
  else {
    while ( ( txSystemStatusAll() != READY ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
      loopAction("WAIT : for rx(MASTER_TRANS) and all tx to be ready, EOf");
  }

  DBG(DBLD, "leave_NoMgtForwardMgtBackward_ClkSet done\n" );

  return status;
}

int leave_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;  

  DBG(DBLD, "leave_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  status |= rxSystemInit(MASTER_TRANSCEIVER);

//  while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for gts_pll to lock, and rx(MASTER_TRANS) to be ready"); /* wait for the external PLL to be locked with the current reference clock and for the RX master transceiver to be ready */

  while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
      loopAction("WAIT : for rx(MASTER_TRANS) to be ready"); /* wait for the RX master transceiver to be ready */

  status |= txSystemResetAll(); /* reset of all active TX PMAs */
//  status |= rxSystemResetAllExcept(MASTER_TRANSCEIVER); /* reset of all active RX PMAs except the master transceiver one */

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feed the LMK_PLL with the clock recovered from the master transceiver */

//    while ( ( isPllLocked() == PLL_NOT_LOCKED) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for gts_pll to lock, and rx(MASTER_TRANS) to be ready, after the switch of rawclk to MASTER_CLK");

    while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
      loopAction("WAIT : for rx(MASTER_TRANS) to be ready, after the switch of rawclk to MASTER_CLK");
  }

  switch (usrclk) {

    case MICTOR_CLK :

      if ( alignModeRead() == CARRIER_INCLUDED ) {
        /* one says to the carrier that the clock sent is good */
        status |= gtsReadySet();
      }

//      while (  ( isPllLocked() == PLL_NOT_LOCKED ) || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) )
//        loopAction("WAIT : for the READY signal from the carrier before setting usrclk\n");

//      while ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//        loopAction("WAIT : for the READY signal from the carrier before setting usrclk\n");

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

  if (raw_clk_changed == TRUE)
  {
    status |= rxPcsReset(MASTER_TRANSCEIVER); /* the RX PCS fifo of the master RocketIO may have become full
    due to mismatches between the local and the recovered clock. This corrects for it. */
    status |= rxPcsInit(MASTER_TRANSCEIVER);
  }

//  while (  ( isPllLocked() == PLL_NOT_LOCKED ) 
//        || ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY )
//        || ( isDcmLocked(usrclk) != DCM_LOCKED )
//        || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//    loopAction("WAIT : for pll, dcm, all rx, and for carrier to be ready, EOf");

//  while ( ( isCarrierClkReady(usrclk) != CLK_CARRIER_READY ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//    loopAction("WAIT : for rx, and for carrier to be ready, EOf");

  while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
    loopAction("WAIT : for rx to be ready, EOf");

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

  status |= txSystemResetAll(); /* reset of all active TX PMAs */
  status |= rxSystemResetAll(); /* reset of all active RX PMAs */

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);
//    status |= XMMRegs_LmkPll_Reset(&XMMRegsDriver);

//    while ( isPllLocked() == PLL_NOT_LOCKED ) 
//      loopAction("WAIT : that gts_pll locks"); /* wait for the external PLL to be locked with the current reference */
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

  if ( alignModeRead() == CARRIER_INCLUDED ) {
    /* one waits for the LMK_PLL of the carrier to unlock */
//    while ( isCarrierClkNotReady(usrclk) != CLK_CARRIER_NOT_READY ) 
//      loopAction("WAIT : for carrier to send the CLK_NOT_READY signal");
  }

  DBG(DBLD, "leave_NoMgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

/********************  fanin-fanout GTS  ***********************/


int faninfanout_MgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "faninfanout_MgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  status |= rxSystemInit(MASTER_TRANSCEIVER);

//  while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//    loopAction("WAIT : for gts pll to lock, and rx of MASTER_TRANSCEIVER to be ready"); /* wait for the external PLL to be locked with the current reference clock and for the RX master transceiver to be ready */

  while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
    loopAction("WAIT : for rx of MASTER_TRANSCEIVER to be ready"); /* wait for the RX master transceiver to be ready */

  status |= txSystemReset(MASTER_TRANSCEIVER);
//  status |= rxSystemResetAllExcept(MASTER_TRANSCEIVER);

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feeds the LMK_PLL with the clock recovered from the master transceiver */

//    while ( ( isPllLocked() == PLL_NOT_LOCKED) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for gts pll to lock, and rx of MASTER_TRANSCEIVER to be ready after the switch of rawclk to MASTER_CLK");

    while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
      loopAction("WAIT : for rx of MASTER_TRANSCEIVER to be ready after the switch of rawclk to MASTER_CLK");
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

//  while ( (isPllLocked() == PLL_NOT_LOCKED) || (isDcmLocked(usrclk) != DCM_LOCKED) )
//    loopAction("WAIT : for a stable input clock and fb dcm locked"); /* one waits for a stable clock */

  status |= txSystemInitAll();
  status |= rxSystemInitAll();

  if (raw_clk_changed == TRUE)
  {
    status |= rxPcsReset(MASTER_TRANSCEIVER); /* the RX PCS fifo of the master RocketIO may have become full
    due to mismatches between the local and the recovered clock. This corrects for it. */
    status |= rxPcsInit(MASTER_TRANSCEIVER);
  }

//  if (block_mode == BLOCKING)
//  {
//    while (  ( isPllLocked() == PLL_NOT_LOCKED ) 
//               || ( isDcmLocked(usrclk) != DCM_LOCKED )
//               || ( txSystemStatusAll() != READY ) 
//               || ( rxSystemStatusAll() != READY ) 
//          ) loopAction("WAIT : that dcm, pll, all rx and tx are ready, blocking mode, EOf");
//  }
//  else {
//    while ( ( isPllLocked() == PLL_NOT_LOCKED) 
//            || ( isDcmLocked(usrclk) != DCM_LOCKED )
//            || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) 
//            || ( txSystemStatusAll() != READY ) )
//            loopAction("WAIT : that dcm, pll, rx(MASTER_TRANS) and all tx are ready, EOf");
//  }

  if (block_mode == BLOCKING)
  {
    while ( ( txSystemStatusAll() != READY ) || ( rxSystemStatusAll() != READY ) )
      loopAction("WAIT : that all rx and tx are ready, blocking mode, EOf");
  }
  else {
    while ( ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) || ( txSystemStatusAll() != READY ) )
      loopAction("WAIT : that rx(MASTER_TRANS) and all tx are ready, EOf");
  }

  DBG(DBLD, "faninfanout_MgtForwardMgtBackward_ClkSet done\n");

  return status;
}

int faninfanout_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) {
  int status = XST_SUCCESS;
  int raw_clk_changed = FALSE;

  DBG(DBLD, "faninfanout_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  status |= rxSystemInit(MASTER_TRANSCEIVER);

//  while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//    loopAction("WAIT : for gts_pll to lock and rx(MASTER_TRANS) to be ready"); 

  while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
    loopAction("WAIT : for rx(MASTER_TRANS) to be ready"); 

  status |= txSystemResetAll(); /* reset of all active TX PMAs */
//  status |= rxSystemResetAllExcept(MASTER_TRANSCEIVER); /* reset of all active RX PMAs except the master transceiver one */

  if ( (rawClkDiagnose() != MASTER_CLK) || (force_set == FORCE_SET) ) 
  {
    raw_clk_changed = TRUE;
    rawClkSet(MASTER_CLK); /* one feed the LMK_PLL with the clock recovered from the master transceiver */

//    while ( ( isPllLocked() == PLL_NOT_LOCKED) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//      loopAction("WAIT : for gts_pll to lock and rx(MASTER_TRANS) to be ready"); 

    while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
      loopAction("WAIT : for rx(MASTER_TRANS) to be ready after the switch of rawclk to MASTER_CLK"); 
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

//  while ( (isPllLocked() == PLL_NOT_LOCKED) || (isDcmLocked(usrclk) != DCM_LOCKED) )
//    loopAction("WAIT : for a stable input clock and fb dcm locked"); /* one waits for a stable clock */

//  status |= txSystemInitAllExcept(MASTER_TRANSCEIVER);

  if (raw_clk_changed == TRUE)
  {
    status |= rxPcsReset(MASTER_TRANSCEIVER); /* the RX PCS fifo of the master RocketIO may have become full
    due to mismatches between the local and the recovered clock. This corrects for it. */
    status |= rxPcsInit(MASTER_TRANSCEIVER);
  }

//  while ( ( isPllLocked() == PLL_NOT_LOCKED ) || ( isDcmLocked(usrclk) != DCM_LOCKED ) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
//    loopAction("WAIT : that dcm, pll, tx(except MASTER_TRANS) and rx(MASTER_TRANS) are ready, EOf");

  while ( rxSystemStatus(MASTER_TRANSCEIVER) != READY )
    loopAction("WAIT : that rx(MASTER_TRANS) is ready, EOf");

  DBG(DBLD, "faninfanout_MgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int faninfanout_NoMgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) 
{
  int status = XST_SUCCESS;

  DBG(DBLD, "faninfanout_NoMgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  status |= txSystemResetAll(); /* reset of all active TX PMAs */
  status |= rxSystemResetAll(); /* reset of all active RX PMAs */

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);

//    while ( isPllLocked() == PLL_NOT_LOCKED )
//      loopAction("WAIT : that gts_pll locks");
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

/********************  root GTS   *****************************/

int root_MgtForwardMgtBackward_ClkSet(int usrclk, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_MgtForwardMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  /* We don't reset the tx rocketIOs, indeed if this function is used by mistake,
  a reset would destroy the clocking of the all GTS tree */

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);
    status |= txSystemResetAll(); 
//    status |= XMMRegs_LmkPll_Reset(&XMMRegsDriver);
    if (force_set == DONT_FORCE_SET) {
      DBG(DBLW, "WARNING : forward = USE_MGT and backward = USE_MGT in root node\n");
      DBG(DBLW, "while rawclk != LOCAL_CLK; it is not the correct way to initialize the GTS tree\n");
      status = XST_FAILURE;
    }

//    while ( isPllLocked() == PLL_NOT_LOCKED )
//      loopAction("WAIT : for external PLL to lock");
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

  status |= txSystemInitAll(); 
  status |= rxSystemResetAll(); 
  status |= rxSystemInitAll(); 

  if (block_mode == BLOCKING) {
    while ( ( txSystemStatusAll() != READY ) || ( rxSystemStatusAll() != READY ) )
      loopAction("WAIT : for tx and rx to be ready, blocking mode, EOf");
  }
/*
  else {
    while ( txSystemStatusAllExcept(MASTER_TRANSCEIVER) != READY ) loopAction("WAIT : for tx (except MASTER transceiver) to be ready, EOf");
  }
*/
  DBG(DBLD, "root_MgtForwardMgtBackward_ClkSet done\n");

  return status;
}

int root_MgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_MgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  status |= txSystemResetAll(); /* reset of all active TX PMAs */
  status |= rxSystemResetAll(); 

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);

//    while ( isPllLocked() == PLL_NOT_LOCKED)
//      loopAction("WAIT : for external PLL to lock");
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

  status |= txSystemInitAll(); /* note that if the system has not been previously reset because rawClkDiagnose() == LOCAL_CLK (see 10 rigs above), this will not reset the tx system */

  if (block_mode == BLOCKING) {
    while ( txSystemStatusAll() != READY )
      loopAction("WAIT : for tx system to be ready, blocking, EOf");
  }
/*
  else {
    while ( txSystemStatusAllExcept(MASTER_TRANSCEIVER) != READY ) loopAction("WAIT : for tx (except MASTER transceiver) to be ready, EOf");
  }
*/
  DBG(DBLD, "root_MgtForwardNoMgtBackward_ClkSet done\n");

  return status;
}

int root_NoMgtForwardNoMgtBackward_ClkSet(int usrclk, int force_set, int block_mode) 
{
  int status = XST_SUCCESS;

  DBG(DBLD, "root_NoMgtForwardNoMgtBackward_ClkSet launched : usrclk = %d\n", usrclk );

  status |= txSystemResetAll(); /* reset of all active TX PMAs */
  status |= rxSystemResetAll(); /* reset of all active RX PMAs */

  if ( (rawClkDiagnose() != LOCAL_CLK) || (force_set == FORCE_SET) ) {
    status |= rawClkSet(LOCAL_CLK);

//    while ( isPllLocked() == PLL_NOT_LOCKED ) ;
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
    status |= root_MgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status |= root_MgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status |= root_NoMgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
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
    status |= leave_MgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status |= leave_MgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
//    DBG(DBLE, "ERROR : forward = BYPASS_MGT, backward = USE_MGT\n");
//    DBG(DBLE, "This mode should never be found in the GTS system\n");
//    status = XST_FAILURE;

    status |= leave_NoMgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status |= leave_NoMgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
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
    status |= faninfanout_MgtForwardMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == USE_MGT) && (backward == BYPASS_MGT) ) {
    status |= faninfanout_MgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
  }
  else if ( (forward == BYPASS_MGT) && (backward == USE_MGT) ) {
    DBG(DBLE, "ERROR : forward = BYPASS_MGT, backward = USE_MGT\n");
    DBG(DBLE, "This mode should never be found in the GTS system\n");
    status = XST_FAILURE;
  }
  else if ( (forward == BYPASS_MGT) && (backward == BYPASS_MGT) ) {
    status |= faninfanout_NoMgtForwardNoMgtBackward_ClkSet(usrclk, force_set, block_mode);
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
      status |= rootClkSet(usrclk, forward, backward, force_set, block_mode);
      break;
    case FANIN_FANOUT :
      status |= faninfanoutClkSet(usrclk, forward, backward, force_set, block_mode);
      break;
    case LEAVE :
      status |= leaveClkSet(usrclk, forward, backward, force_set, block_mode);
      break;
    default :
      DBG(DBLE, "ERROR : out of range error on GTStype values in function clkSet\n");
      status |= XST_FAILURE;
      break; 
  }

  return status;
}

#undef DBG
