#include <unistd.h>
#include "xmmregs_i.h"
#include "xtdc.h"
#include "debug.h"
#include "align.h"

#define DBG DBG_ALIGN_TDC

/************************************************************

		LOW LEVEL FUNCTIONS

************************************************************/

void tdcNc(unsigned int delay)
{
  xtdc_Meas1NonCalibrated_Measure(&XSPIDriver, &XMMRegsDriver, delay, 1);
}

void tdcC(unsigned int delay)
{
  xtdc_Meas1Calibrated_Measure(&XSPIDriver, &XMMRegsDriver, delay, 1);
}

void tdc2C(unsigned int delay)
{
  xtdc_Meas2Calibrated_Measure(&XSPIDriver, &XMMRegsDriver, delay, 1);
}

void cal(void)
{
  xtdc_Meas1Calibrated_Calibrate(&XSPIDriver, &XMMRegsDriver, 10, 20);
}

void doOne(void)
{
  xtdc_DoOneSweep(&XMMRegsDriver, TDC_DEBUG); 
}

void printCr(void)
{
  XSPI_PrintCr(&XSPIDriver);
}

void printSr(void)
{
  XSPI_PrintSr(&XSPIDriver);
}

int tdcRegRead(unsigned int index)
{
  unsigned int valreg;

  valreg = XSPI_tdcgp2_RegRead(&XSPIDriver, index);
  eprintf(FIRST_ARG, "%u : 0x%08X\n", index, valreg); estrcpy();
  return XST_SUCCESS;
}
    
/************************************************************

		HIGHER LEVEL FUNCTIONS

************************************************************/

int tdcComSet(int debug, int delay)
{
  int status = XST_SUCCESS;

  status  |= xtdc_CommunicationSet(&XMMRegsDriver, debug, delay);

  return status;
}

static tdcgp2_reg_t Reg1Tdc;

int tdcSet(int meas, int debug, int delay)
{
  int status = XST_SUCCESS;

  status  |= xtdc_CommunicationSet(&XMMRegsDriver, debug, delay);
  Reg1Tdc = xtdc_Configure(&XSPIDriver, meas);

  return status;
}

int tdcStop(void)
{
  return XST_SUCCESS; 
}

int tdcMeas(int meas, int debug, int nmes)
{
  int status = XST_SUCCESS;
  int cnt;

  for(cnt=0;cnt<nmes;cnt++)
  {
    status |= xtdc_NewMeasure(&XSPIDriver, &Reg1Tdc, meas);
    status |= xtdc_DoOneSweep(&XMMRegsDriver, debug);
    status |= xtdc_MeasureRead(&XSPIDriver, meas);	
  }

  return status;
}

#undef DBG
