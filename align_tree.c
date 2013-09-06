#include <math.h>
#include "align.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xdebug_i.h"
#include "xmmregs_RocketIO.h"
#include "debug.h"

#define DBG DBG_ALIGN_TREE

int treeSetup(int step)
{
  int status = XST_SUCCESS;
  int iscomma;

  DBG(DBLD, "treeSetup %d started\n", step);

  switch (step)
  {
    case 1 :

    XMMRegs_RocketIO_TriggerRstCarrier_Unset(&XMMRegsDriver, 0);

    status |= transCon(TRANSCEIVER_IS_CONNECTED, TRANSCEIVER_IS_CONNECTED, TRANSCEIVER_IS_CONNECTED, TRANSCEIVER_IS_CONNECTED);
    status |= transGtsTree(TRANSCEIVER_IS_IN_GTS_TREE, TRANSCEIVER_IS_IN_GTS_TREE, TRANSCEIVER_IS_IN_GTS_TREE, TRANSCEIVER_IS_IN_GTS_TREE);
    status |= transDigitizer(TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER, TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER, TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER, TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER);

//    status |= XMMRegs_Update(&XMMRegsDriver); // commented because GTS_LEAF on ML507 needs trigger_rst_carrier bit at zero

    /* sends comma for alignment */
    status |= XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, 0, ZERO_VECTOR, COMMA, COMMA_OFF, COMMA_ON);

    /* at first, considers itself as a root */  
    status |= muxExtSet(ROOT, USE_MGT, USE_MGT);
    status |= allRegMuxPathSet();
    status |= clkSet(ROOT, LOCAL_CLK, USE_MGT, USE_MGT, FORCE_SET, NON_BLOCKING);
    break;

    case 2 :

    iscomma = COMMA_NOT_DETECTED;
    DBG(DBLD, "comma detection starts ...");

    while (iscomma == COMMA_NOT_DETECTED)
    {
      iscomma = COMMA_DETECTED;

      if ( XMMRegs_RocketIO_CommaAlign_Diagnose (&XMMRegsDriver, 0) != COMMA_DETECTED ) iscomma = COMMA_NOT_DETECTED;
    }

    DBG(DBLD, ">| done\n");

    break;

    case 3 :

    XMMRegs_RocketIO_TriggerRstCarrier_Unset(&XMMRegsDriver, 0);

//    if ( (isPllLocked() == PLL_NOT_LOCKED) || ( rxSystemStatus(MASTER_TRANSCEIVER) != READY ) )
    if (rxSystemStatus(MASTER_TRANSCEIVER) != READY)
    {
      DBG(DBLD, "it considers itself as a ROOT (external PLL is NOT locked or RX master transceiver is NOT ready)\n");
      status |= muxExtSet(ROOT, USE_MGT, USE_MGT);
      status |= allRegMuxPathSet();
      status |= clkSet(ROOT, LOCAL_CLK, USE_MGT, USE_MGT, DONT_FORCE_SET, NON_BLOCKING);
    }
    else
    {
      DBG(DBLD, "it considers itself as a LEAF or a FANIN-FANOUT (external PLL is locked and RX master transceiver is ready)\n");
      status |= muxExtSet(FANIN_FANOUT, USE_MGT, USE_MGT);
      status |= allRegMuxPathSet();
      status |= clkSet(FANIN_FANOUT, RECOVERED_CLK, USE_MGT, USE_MGT, FORCE_SET, NON_BLOCKING);
    }

    /* sends COMMA again, indeed maybe the TX are not ready yet and have to align */
    status |= XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, 0, ZERO_VECTOR, COMMA, COMMA_OFF, COMMA_ON);

    break;

    case 4 :

    iscomma = COMMA_NOT_DETECTED;
    DBG(DBLD, "comma detection starts ...");

    while (iscomma == COMMA_NOT_DETECTED)
    {
      iscomma = COMMA_DETECTED;

      if ( XMMRegs_RocketIO_CommaAlign_Diagnose (&XMMRegsDriver, 0) != COMMA_DETECTED ) iscomma = COMMA_NOT_DETECTED;
    }

    DBG(DBLD, ">| done\n");

    break;

    default :

    status = XST_FAILURE;
    break;
  }

  DBG(DBLD, "treeSetup %d done\n", step);

  return status;
}

int treeStart(void) {
  int status = XST_SUCCESS;
  unsigned int msb, lsb;
  int t;

  /* status |= XMMRegs_DataPath_UseSync_UnSet(&XMMRegsDriver); */
  msb = *((unsigned int *) CARD_NUMBER_ADDRESS);
  if (msb >= CARD_NUMBER_ERROR) {
    DBG(DBLE, "ERROR : card number not set at address %p, or not readable\n", CARD_NUMBER_ADDRESS);
    msb = CARD_NUMBER_ERROR;
    status = XST_FAILURE;
  }

  /* everyone sends information about itself */
  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_0; t++) {
    lsb = (unsigned int) t;
    DBG(DBLD, "trans = %d; msb = %u, lsb = %u\n", t, msb, lsb);
    status |= XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, t, msb, lsb, COMMA_OFF, COMMA_OFF);
  }

  return status;
}

unsigned int treeRead(int transceiver) {
  return XMMRegs_RocketIO_RxMgtdata_Read(&XMMRegsDriver, transceiver);
}

int treeStop(void) {
  int status = XST_SUCCESS;
  int t;

  /* status |= XMMRegs_DataPath_UseSync_Set(&XMMRegsDriver); */

  for (t = TRANSCEIVER_0; t <= TRANSCEIVER_0; t++) {
    status |= XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, t, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);
  }

  return status;
}

#undef DBG
