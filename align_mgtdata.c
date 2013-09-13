#include "align.h"
#include "debug.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xmmregs_RocketIO.h"

#define DBG DBG_ALIGN_MGTDATA

void mgtDataCommaSet(void) 
{
  XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);
}

void mgtDataCommaUnSet(void) 
{
  XMMRegs_RocketIO_MgtData_Set(&XMMRegsDriver, ZERO_VECTOR, ZERO_VECTOR, COMMA_OFF, COMMA_OFF);
}

#undef DBG
