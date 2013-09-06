#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "xmmregs_i.h"
#include "debug.h"

#define TRUE 1
#define FALSE 0

#define DBG DBG_GTSINIT

/* it initializes the mmregs register at the start values
XMMRegsDriver and XSPIDriver should be initialized before using the function */
int gtsInit(void)
{
  int status = XST_SUCCESS;
  
  status |= XMMRegs_Init(&XMMRegsDriver);
   
  return status;
}

/* It stops and reinitializes the driver */
int gtsReset(void)
{
  int status = XST_SUCCESS;
  
  status |= XMMRegs_Reset(&XMMRegsDriver);
   
  return status;
}

/************************************************************

	       USER FUNCTIONS

************************************************************/

int transCon(int con0, int con1, int con2, int con3)
{
  int status = XST_SUCCESS;
  int con[4] = {con0, con1, con2, con3};
  int trans;
  
  for (trans = TRANSCEIVER_0; trans <= TRANSCEIVER_3; trans++) {
    if (con[trans])
      status |= XMMRegs_Transceiver_Connect(&XMMRegsDriver, trans);
    else
      status |= XMMRegs_Transceiver_DisConnect(&XMMRegsDriver, trans);
  }
  
  return status;
}

int transGtsTree(int con0, int con1, int con2, int con3)
{
  int status = XST_SUCCESS;
  int con[4] = {con0, con1, con2, con3};
  int trans;
  
  for (trans = TRANSCEIVER_0; trans <= TRANSCEIVER_3; trans++) {
    if (con[trans])
      status |= XMMRegs_TransceiverGtsTree_Insert(&XMMRegsDriver, trans);
    else
      status |= XMMRegs_TransceiverGtsTree_Remove(&XMMRegsDriver, trans);
  }
  
  return status;
}

int transDigitizer(int con0, int con1, int con2, int con3)
{
  int status = XST_SUCCESS;
  int con[4] = {con0, con1, con2, con3};
  int trans;
  
  for (trans = TRANSCEIVER_0; trans <= TRANSCEIVER_3; trans++) {
    if (con[trans])
      status |= XMMRegs_TransceiverDigitizer_Connect(&XMMRegsDriver, trans);
    else
      status |= XMMRegs_TransceiverDigitizer_DisConnect(&XMMRegsDriver, trans);
  }
  
  return status;
}

int gtsUpdate(void)
{
  return XMMRegs_Update(&XMMRegsDriver);
}

int conIs(void)
{
  int trans, con;
  
  for (trans = TRANSCEIVER_0; trans <= TRANSCEIVER_3; trans++)
  {
    con = XMMRegs_IsTransceiverConnected(&XMMRegsDriver, trans);
    if (con == TRUE) {
      DBG(DBLD, "transceiver %d is considered connected : %d\n", trans, con);
    }
    else {
      DBG(DBLD, "transceiver %d is considered disconnected : %d\n", trans, con);
    }
  }
    
  return XST_SUCCESS;
}

int gtsIs(void)
{
  int trans, gts;
  
  for (trans = TRANSCEIVER_0; trans <= TRANSCEIVER_3; trans++)
  {
    gts = XMMRegs_IsTransceiverInGtsTree(&XMMRegsDriver, trans);
    if (gts == TRUE) {
      DBG(DBLD, "transceiver %d is considered in GTS tree : %d\n", trans, gts);
    }
    else {
      DBG(DBLD, "transceiver %d is considered NOT in GTS tree : %d\n", trans, gts);
    }
  }
    
  return XST_SUCCESS;
}

unsigned int regRead(unsigned int offset)
{
  return XMMRegs_ReadRegister(&XMMRegsDriver, offset); 
}

int printHex(unsigned int offset)
{
  XMMRegs_PrintHex(&XMMRegsDriver, offset);
  return XST_SUCCESS;
}

int printBinary(unsigned int offset)
{
  XMMRegs_PrintBinary(&XMMRegsDriver, offset);
  return XST_SUCCESS;
}

unsigned int fullAddressRegRead(void *addr)
{
  unsigned int val;
  val = *((unsigned int *) addr);
  return val; 
}

/* it sends a presence message through internet when requested
it should work even when the DBGs are disabled */  
unsigned int gtsTest(void)
{
  unsigned int card_number = *((unsigned int *) CARD_NUMBER_ADDRESS);
  sprintf(global_log, "gts %u is visible\n",  card_number);
  logAnswer();
  return card_number;
}

#undef DBG
