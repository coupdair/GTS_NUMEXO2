#include "xmmregs_i.h"
#include "xmmregs_RocketIO.h"

int rxSystemIsReady(int transceiver)
{
  int system_status;

  system_status = XMMRegs_RocketIO_RxSystem_Status(&XMMRegsDriver);

  if (system_status == READY)
    return XST_SUCCESS;
  else
    return XST_FAILURE;
}
