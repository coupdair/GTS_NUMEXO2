#include "xmmregs_i.h"
#include "xmmregs_RocketIO.h"
/*
int LoopbackSet(void)
{
//  XMMRegs_RocketIO_SerialLoopback_Set(&XMMRegsDriver, 0, SERIAL_LOOPBACK_ON);

  return XST_SUCCESS;
}
*/
/*
int LoopbackUnSet(void)
{
//  XMMRegs_RocketIO_SerialLoopback_Set(&XMMRegsDriver, 0, SERIAL_LOOPBACK_OFF);

  return XST_SUCCESS;
}
*/
int drpRead(int transceiver, unsigned char DADDR)
{
  XMMRegs_RocketIO_DrpWord_Read(&XMMRegsDriver, transceiver, DADDR);

  return XST_SUCCESS;
}

int rxSystemIsReady(int transceiver)
{
  int system_status;

  system_status = XMMRegs_RocketIO_RxSystem_Status(&XMMRegsDriver, transceiver);

  if (system_status == READY)
    return XST_SUCCESS;
  else
    return XST_FAILURE;
}
