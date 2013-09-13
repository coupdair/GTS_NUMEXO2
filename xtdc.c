#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 
#include "xdebug.h"
#include "xoutput.h"
#include "xtdc.h"

#define DBG DBG_XTDC
#define TPT TPT_XTDC

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

/************************************************************

		LOW-LEVEL FUNCTIONS

************************************************************/

int xtdc_CommunicationSet(XMMRegs *InstancePtr, int debug_mode, unsigned int delay)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "communication setting...");

  /* enables tdc */
  XMMRegs_MuxInOut_Tdc_Disable(InstancePtr);
  XMMRegs_MuxInOut_Tdc_Enable(InstancePtr);

  /* if debug_mode == NO_TDC_DEBUG, TdcDebug is bypassed */
  switch (debug_mode) {
    case TDC_DEBUG :
      status |= XMMRegs_TdcDebug_DebugMode_Launch(InstancePtr); 
      status |= XMMRegs_TdcDebug_Delay_Set(InstancePtr, delay);
      break;
    case NO_TDC_DEBUG :
      status |= XMMRegs_TdcDebug_DebugMode_Remove(InstancePtr); 
      break;
    default :
      DBG(DBLE, "ERROR : debug_mode is out of range in xtdc_CommunicationSet\n");
      status = XST_FAILURE;
      break;
  }

  DBG(DBLD, "done\n");

  return status;
}

tdcgp2_reg_t xtdc_Configure(XSPI *InstanceSPI, int measure)
{
  tdcgp2_reg_t r1;

  DBG(DBLD, "initializing SPI for tdc...");  
  XSPI_tdcgp2_InitSPI(InstanceSPI);
  DBG(DBLD, "done\n");

  DBG(DBLD, "resetting tdc...");
  XSPI_tdcgp2_PowerOnReset(InstanceSPI);
  DBG(DBLD, "done\n");

  DBG(DBLD, "configuring tdc...");

  switch (measure) {
    case MEAS1_NON_CALIBRATED :
      r1 = XSPI_tdcgp2_Meas1NonCalibrated_Configure(InstanceSPI);
      break;
    case MEAS1 :
      r1 = XSPI_tdcgp2_Meas1Calibrated_Configure(InstanceSPI);
      /* XSPI_tdcgp2_CalTDC(InstanceSPI); */
      break;
    case MEAS2 :
      r1 = XSPI_tdcgp2_Meas2Calibrated_Configure(InstanceSPI);
      /* XSPI_tdcgp2_CalTDC(InstanceSPI); */
      break;
    case CALIBRATION :
      r1 = XSPI_tdcgp2_MeasCalibration_Configure(InstanceSPI);
      break;
    default :
      break; 
  }

  DBG(DBLD, "done\n");

  return r1;
}

int xtdc_NewMeasure(XSPI *InstanceSPI, tdcgp2_reg_t *r1, int measure)
{
  int status = XST_SUCCESS;

  switch (measure) {
    case MEAS1_NON_CALIBRATED :
      status |= XSPI_tdcgp2_InitDataReg(InstanceSPI);
      r1->r1.Hit1 = 0; 
      r1->r1.Hit2 = 1;
      status |= XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1->uint);
      break;
    case MEAS1 :
      status |= XSPI_tdcgp2_InitDataReg(InstanceSPI);
      r1->r1.Hit1 = 1;
      r1->r1.Hit2 = 0;
      status |= XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1->uint);
      break;
    case MEAS2 :
      status |= XSPI_tdcgp2_InitDataReg(InstanceSPI);
      r1->r1.Hit1 = 1;
      r1->r1.Hit2 = 2; 
      status |= XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1->uint);
      status |= XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1->uint);
      break;
    case CALIBRATION :
      XSPI_tdcgp2_InitDataReg(InstanceSPI);
      r1->r1.Hit1 = 7;
      r1->r1.Hit2 = 6;
      XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1->uint);
      break; 
    default :
      DBG(DBLE, "ERROR : measure is out of range in xtdc_NewMeasure\n");
      status = XST_FAILURE;
      break;
  }

  status |= microsleep(WAIT_IN_MICROSEC);

  return status;
}

int xtdc_DoOneSweep(XMMRegs *InstancePtr, int debug)
{
  int status = XST_SUCCESS;

  switch (debug) {
    case TDC_DEBUG :
      status = XMMRegs_TdcDebug_OneSweep_Do(InstancePtr);
      break;
    case NO_TDC_DEBUG :
      XMMRegs_DataPath_OneSweep_Do(InstancePtr);
      break;
    default :
      DBG(DBLE, "ERROR : debug is out of range in xtdc_DoOneSweep\n");
      status = XST_FAILURE;
      break; 
  }

  status |= microsleep(WAIT_IN_MICROSEC);

  return status;
}

int xtdc_MeasureRead(XSPI *InstanceSPI, int measure)
{
  int status = XST_SUCCESS;
  double val;
  tdcgp2_reg_t reg;

  switch (measure) {
    case MEAS1_NON_CALIBRATED :
      reg.uint = XSPI_tdcgp2_RegRead(InstanceSPI, 0);
      TPT("%u : \n", reg.res.NC.Meas);
      TPT("stop1 - start = %f ns\n", ( (float) (reg.res.NC.Meas * 65) ) / 1000.0);
      break;
    case MEAS1 :
      reg.uint = XSPI_tdcgp2_RegRead(InstanceSPI, 0);
      /* TPT("%f ns\n", ( ((float)reg.res.C.I) + ((float) reg.res.C.D ) / 65536.0 ) * 2 * 26.0 / 200.0 * 1000.0); */ 
      val = ( ((double)reg.res.C.I) + ((double) reg.res.C.D ) / 65536.0 ) * 2;
      TPT("%f\n", val);  
      /*     DBG(DBLI, "%f\n", ((float)(reg.res.C.D * 13)) / 25.0 ); */
      DBG(DBLD, "meas1 = %f\n", val);
      break;
    case MEAS2 :
      reg.uint = XSPI_tdcgp2_RegRead(InstanceSPI, 0);
      /* TPT("%f ns\n", ( ((float)reg.res.C.I) + ((float) reg.res.C.D ) / 65536.0 ) * 26.0 / 200.0 * 1000.0); */ 
      val = ( ((double)reg.res.C.I) + ((double) reg.res.C.D ) / 65536.0 );
      if ( ( val * 26.0 / 200000.0 ) > WAIT_IN_MICROSEC )
        DBG(DBLE, "ERROR : you read the value before the arriving of the STOP pulse\n");
      TPT("%f\n", val); 
      DBG(DBLD, "meas2 = %f\n", val);
      break;
    case CALIBRATION :
      reg.uint = XSPI_tdcgp2_RegRead(InstanceSPI, 0);
      TPT("%f\n", ( ((float)reg.res.C.I) + ((float) reg.res.C.D ) / 65536.0 ) );
      break; 
    default :
      DBG(DBLE, "ERROR : measure is out of range in XSPI_Measurement_Read\n");
      status = XST_FAILURE;
      break;
  }

  return status;
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

/* non-calibrated test */
int xtdc_Meas1NonCalibrated_Measure(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes)
{
  int status = XST_SUCCESS;
  int cnt;
  tdcgp2_reg_t r1;

  status |= xtdc_CommunicationSet(InstancePtr, TDC_DEBUG, delay);
  r1      = xtdc_Configure(InstanceSPI, MEAS1_NON_CALIBRATED);

  for(cnt=0;cnt<nmes;cnt++)
  {
    status |= xtdc_NewMeasure(InstanceSPI, &r1, MEAS1_NON_CALIBRATED);
    status |= xtdc_DoOneSweep(InstancePtr, TDC_DEBUG);
    status |= xtdc_MeasureRead(InstanceSPI, MEAS1_NON_CALIBRATED);	
  }	

  return status;
}

/* calibrated test */
int xtdc_Meas1Calibrated_Measure(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes)
{
  int status = XST_SUCCESS;
  int cnt;
  tdcgp2_reg_t r1;

  status |= xtdc_CommunicationSet(InstancePtr, TDC_DEBUG, delay);
  r1      = xtdc_Configure(InstanceSPI, MEAS1);

  for(cnt=0;cnt<nmes;cnt++)
  {
    status |= xtdc_NewMeasure(InstanceSPI, &r1, MEAS1);
    status |= xtdc_DoOneSweep(InstancePtr, TDC_DEBUG);
    status |= xtdc_MeasureRead(InstanceSPI, MEAS1);	
  }	

  return status;
}

/* calibrated test */
int xtdc_Meas1Calibrated_Calibrate(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes)
{
  int status = XST_SUCCESS;
  int cnt;
  tdcgp2_reg_t r1;

  status |= xtdc_CommunicationSet(InstancePtr, TDC_DEBUG, delay);
  r1      = xtdc_Configure(InstanceSPI, CALIBRATION);

  for(cnt=0;cnt<nmes;cnt++)
  {
    status |= xtdc_NewMeasure(InstanceSPI, &r1, CALIBRATION);
    status |= xtdc_DoOneSweep(InstancePtr, TDC_DEBUG);
    status |= xtdc_MeasureRead(InstanceSPI, CALIBRATION);	
  }	

  return status;
}

/* calibrated test, measurement range 2 */
int xtdc_Meas2Calibrated_Measure(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes)
{
  int status = XST_SUCCESS;
  int cnt;
  tdcgp2_reg_t r1;

  status |= xtdc_CommunicationSet(InstancePtr, TDC_DEBUG, delay);
  r1      = xtdc_Configure(InstanceSPI, MEAS2);

  for(cnt=0;cnt<nmes;cnt++)
  {
    status |= xtdc_NewMeasure(InstanceSPI, &r1, MEAS2);
    status |= xtdc_DoOneSweep(InstancePtr, TDC_DEBUG);
    status |= xtdc_MeasureRead(InstanceSPI, MEAS2);	
  }	

  return status;
}

#undef DBG
#undef TPT
