#include "align.h"
#include "debug.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xmmregs_RocketIO.h"

#define DBG DBG_ALIGN_MGTDATA

int mgtDataCommaSet(void) 
{
  int status = XST_SUCCESS;
  
  status |= XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, 0, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);
  
  return status;
}

int mgtDataCommaUnSet(void) 
{
  int status = XST_SUCCESS;
  
  status |= XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, 0, ZERO_VECTOR, ZERO_VECTOR, COMMA_OFF, COMMA_OFF);
  
  return status;
}

#undef DBG
