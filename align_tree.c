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

    XMMRegs_RocketIO_TriggerRstCarrier_Unset(&XMMRegsDriver);

    status  = transCon(TRANSCEIVER_IS_CONNECTED, TRANSCEIVER_IS_CONNECTED, TRANSCEIVER_IS_CONNECTED, TRANSCEIVER_IS_CONNECTED);
    status |= transGtsTree(TRANSCEIVER_IS_IN_GTS_TREE, TRANSCEIVER_IS_IN_GTS_TREE, TRANSCEIVER_IS_IN_GTS_TREE, TRANSCEIVER_IS_IN_GTS_TREE);
    status |= transDigitizer(TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER, TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER, TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER, TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER);

    /* sends comma for alignment */
    XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, ZERO_VECTOR, COMMA, COMMA_OFF, COMMA_ON);
 
    status |= muxExtSet(LEAVE, USE_MGT, USE_MGT);
    status |= allRegMuxPathSet();
    status |= clkSet(ROOT, LOCAL_CLK, USE_MGT, USE_MGT, FORCE_SET, NON_BLOCKING); // sets the clock as if it was a ROOT
    break;

    case 2 :

    iscomma = COMMA_NOT_DETECTED;
    DBG(DBLD, "comma detection starts ...");

    while (iscomma == COMMA_NOT_DETECTED)
    {
      iscomma = COMMA_DETECTED;

      if ( XMMRegs_RocketIO_CommaAlign_Diagnose(&XMMRegsDriver) != COMMA_DETECTED ) iscomma = COMMA_NOT_DETECTED;
    }

    DBG(DBLD, ">| done\n");

    break;

    case 3 :

    XMMRegs_RocketIO_TriggerRstCarrier_Unset(&XMMRegsDriver);

    DBG(DBLD, "it considers itself as a LEAF or a FANIN-FANOUT\n");
    status  = muxExtSet(LEAVE, USE_MGT, USE_MGT);
    status |= allRegMuxPathSet();
    status |= clkSet(FANIN_FANOUT, RECOVERED_CLK, USE_MGT, USE_MGT, FORCE_SET, NON_BLOCKING); // sets the clock as if it was a FANIN-FANOUT

    /* sends COMMA again, indeed maybe the TX are not ready yet and have to align */
    XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, ZERO_VECTOR, COMMA, COMMA_OFF, COMMA_ON);

    break;

    case 4 :

    iscomma = COMMA_NOT_DETECTED;
    DBG(DBLD, "comma detection starts ...");

    while (iscomma == COMMA_NOT_DETECTED)
    {
      iscomma = COMMA_DETECTED;

      if ( XMMRegs_RocketIO_CommaAlign_Diagnose (&XMMRegsDriver) != COMMA_DETECTED ) iscomma = COMMA_NOT_DETECTED;
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

int treeStart(void)
{
  unsigned int msb;

  msb = *((unsigned int *)CARD_NUMBER_ADDRESS);

  XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, msb, 0, COMMA_OFF, COMMA_OFF);

  return XST_SUCCESS;
}

unsigned int treeRead(int transceiver)
{
  if(transceiver < 0 || transceiver > 3) {
    return XST_FAILURE;
  }
  return XMMRegs_RocketIO_RxMgtdata_Read(&XMMRegsDriver,transceiver);
}

int treeStop(void)
{
  XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);

  return XST_SUCCESS;
}

#undef DBG
