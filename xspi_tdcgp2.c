#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xspi_tdcgp2.h"
#include "xspi.h"
#include "xdebug_l.h"

#define DBG XSPI_TDCGP2_DBG

static unsigned int rr_sizes[TDCGP2_RREG_NUMBER] = {4,4,4,4,2,1};
static unsigned int wr_sizes[TDCGP2_WREG_NUMBER] = {3,3,3,3,3,3};

/************************************************************

		LOW LEVEL FUNCTIONS

************************************************************/

int XSPI_tdcgp2_InitSPI(XSPI *InstanceSPI)
{
  XSPI_cr_t *cr;
  
  cr = (XSPI_cr_t *)(InstanceSPI->BaseAddress + XSPI_CR_OFFSET);
  cr->MasterTransactionInhibit = 0;
  cr->ManualSlaveSelectAssertionEnable = 1;
  cr->RxFifoReset = 1;
  cr->TxFifoReset = 1;
  cr->Cpha = 1;
  cr->Cpol = 0;
  cr->Master = 1;
  cr->Spe = 1;
  cr->Loop = 0;
  cr->RxFifoReset = 0;
  cr->TxFifoReset = 0;
  return XST_SUCCESS;
}

/* reads the TDC register number index */
unsigned int XSPI_tdcgp2_RegRead(XSPI *InstanceSPI, unsigned int index)
{
	
	int i,r,res=0;
        void *sr;
        
	if(index >= TDCGP2_RREG_NUMBER)
	{
		DBG(DBLE,"WARNING : XSPI_tdcgp2_RegRead: index %u is out of range\n", index);
		return 0;
	}
        
        sr = InstanceSPI->BaseAddress + XSPI_SR_OFFSET;

	TDCGP2_SELECT(0);
	TDCGP2_SEND(TDCGP2_CMD_READ | index);
	while(((XSPI_sr_t *)sr)->RxEmpty);
	r = TDCGP2_RECV();
	
	for(i = rr_sizes[index] - 1; i >= 0; i--)
	{
		TDCGP2_SEND(0);
		while(((XSPI_sr_t *)sr)->RxEmpty);
		r = TDCGP2_RECV();
                res |= ( (r & 0xFF) << (i * 8) );
	}
	TDCGP2_UNSELECT(0);
	return res;
}

/* writes to the TDC register number 'index' the value 'value' */
int XSPI_tdcgp2_RegWrite(XSPI *InstanceSPI, unsigned int index, unsigned int value)
{
  int r,i;
  void *sr;
  unsigned int valsent;
  
  if(index >= TDCGP2_WREG_NUMBER)
  {
    DBG(DBLE,"WARNING : XSPI_tdcgp2_RegWrite(): index %u is out of range!\n", index);
    return XST_FAILURE;
  } 
  
  sr = InstanceSPI->BaseAddress + XSPI_SR_OFFSET;
  
  TDCGP2_SELECT(0);
  TDCGP2_SEND(TDCGP2_CMD_WRITE | index);
  while (((XSPI_sr_t *)sr)->RxEmpty);
  r = TDCGP2_RECV();
  
  DBG(DBLD, "complete value to be sent : 0x%08X\n", value);
  for(i = wr_sizes[index]-1; i >=0; i--)
  {
    valsent = (value << ((3-i)*8));
    valsent = (valsent >> 24);
    TDCGP2_SEND(valsent);
    while (((XSPI_sr_t *)sr)->RxEmpty);
    r = TDCGP2_RECV();
  }
  TDCGP2_UNSELECT(0);
  return XST_SUCCESS;
}

int XSPI_tdcgp2_PowerOnReset(XSPI *InstanceSPI)
{
	unsigned int r;
	TDCGP2_SELECT(0);
	TDCGP2_SEND(TDCGP2_CMD_PORESET);
        while(((XSPI_sr_t *)(InstanceSPI->BaseAddress+XSPI_SR_OFFSET))->RxEmpty);
	r = TDCGP2_RECV();
	TDCGP2_UNSELECT(0);	
        
        return XST_SUCCESS;
}

int XSPI_tdcgp2_InitDataReg(XSPI *InstanceSPI)
{
	unsigned int r;
	TDCGP2_SELECT(0);
	TDCGP2_SEND(TDCGP2_CMD_INIT);
        while(((XSPI_sr_t *)(InstanceSPI->BaseAddress+XSPI_SR_OFFSET))->RxEmpty);
	r = TDCGP2_RECV();
	TDCGP2_UNSELECT(0);	
        
        return XST_SUCCESS;
}


int XSPI_tdcgp2_CalResonator(XSPI *InstanceSPI)
{
	unsigned int r;
	TDCGP2_SELECT(0);
	TDCGP2_SEND(TDCGP2_CMD_SCRE);
        while(((XSPI_sr_t *)(InstanceSPI->BaseAddress+XSPI_SR_OFFSET))->RxEmpty);
	r = TDCGP2_RECV();
	TDCGP2_UNSELECT(0);
        
        return XST_SUCCESS;
}

int XSPI_tdcgp2_CalTDC(XSPI *InstanceSPI)
{
	unsigned int r;
	TDCGP2_SELECT(0);
	TDCGP2_SEND(TDCGP2_CMD_SCTD);
        while(((XSPI_sr_t *)(InstanceSPI->BaseAddress+XSPI_SR_OFFSET))->RxEmpty);
	r = TDCGP2_RECV();
	TDCGP2_UNSELECT(0);

        return XST_SUCCESS;
}

/************************************************************

		CONFIGURE FUNCTIONS

************************************************************/

tdcgp2_reg_t XSPI_tdcgp2_Meas1NonCalibrated_Configure(XSPI *InstanceSPI)
{
  
  tdcgp2_reg_t reg,r1;
  
  reg.uint = TDCGP2_REG0_DEFAULT;
  reg.r0.MRange2    = 0; /* MEAS RANGE 1 */
  reg.r0.SelClkT    = 1; /* HIGH SPEED CLOCK */
  reg.r0.StartClkHS = 1; /* only H Speed clock used */
  reg.r0.Calibrate  = 0; /* uncalibrated */
  reg.r0.DisAutoCal = 1; /* autocalibration disabled */
  reg.r0.NegStart   = 0;   /* rising */
  reg.r0.NegStop1   = 0;   /* rising */
  reg.r0.NegStop2   = 0;   /* rising */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 0, reg.uint);
	
  r1.uint = TDCGP2_REG1_DEFAULT;
  r1.r1.HitIn1 = 1;
  r1.r1.HitIn2 = 0;
  r1.r1.Hit1   = 5;  /* ALU disabled */
  r1.r1.Hit2   = 5;  /* ALU disabled */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1.uint);
	
  reg.uint = TDCGP2_REG2_DEFAULT;
  reg.r2.RfEdge1 = 0; /* one edge */
  reg.r2.RfEdge2 = 0; /* one edge */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 2, reg.uint);
  
  reg.uint = TDCGP2_REG3_DEFAULT;  
  reg.r3.EnErrVal = 1; /* timeout error detection */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 3, reg.uint);
  
  XSPI_tdcgp2_RegWrite(InstanceSPI, 4, TDCGP2_REG4_DEFAULT);
  XSPI_tdcgp2_RegWrite(InstanceSPI, 5, TDCGP2_REG5_DEFAULT); 
  
  return r1;
}

tdcgp2_reg_t XSPI_tdcgp2_Meas1Calibrated_Configure(XSPI *InstanceSPI)
{
  
  tdcgp2_reg_t reg,r1;
  
  reg.uint = TDCGP2_REG0_DEFAULT;
  reg.r0.MRange2    = 0; /* MEAS RANGE 1 */
  reg.r0.SelClkT    = 1; /* HIGH SPEED CLOCK */
  reg.r0.StartClkHS = 1; /* only H Speed clock used */
  reg.r0.CalResN    = 0; /* 2 periods used for calibration */
  reg.r0.ClkHSDiv   = 1; 
  reg.r0.Calibrate  = 1; /* calibrated */
  reg.r0.DisAutoCal = 0; /* autocalibration enabled */
  reg.r0.NegStart   = 0;   /* rising */
  reg.r0.NegStop1   = 0;   /* rising */
  reg.r0.NegStop2   = 0;   /* rising */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 0, reg.uint);
	
  r1.uint = TDCGP2_REG1_DEFAULT;
  r1.r1.HitIn1 = 1;
  r1.r1.HitIn2 = 0;
  r1.r1.Hit1   = 5;  /* ALU disabled */
  r1.r1.Hit2   = 5;  /* ALU disabled */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1.uint);
	
  reg.uint = TDCGP2_REG2_DEFAULT;
  reg.r2.RfEdge1 = 0; /* one edge */
  reg.r2.RfEdge2 = 0; /* one edge */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 2, reg.uint);
  
  reg.uint = TDCGP2_REG3_DEFAULT;  
  reg.r3.EnErrVal = 1; /* timeout error detection */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 3, reg.uint);
  
  XSPI_tdcgp2_RegWrite(InstanceSPI, 4, TDCGP2_REG4_DEFAULT);
  
  XSPI_tdcgp2_RegWrite(InstanceSPI, 5, TDCGP2_REG5_DEFAULT);
  
  return r1;
}

tdcgp2_reg_t XSPI_tdcgp2_Meas2Calibrated_Configure(XSPI *InstanceSPI)
{
  
  tdcgp2_reg_t reg,r1;

  reg.uint = TDCGP2_REG0_DEFAULT;
  reg.r0.MRange2    = 1; /* MEAS RANGE 2 */
  reg.r0.SelClkT    = 1; /* HIGH SPEED CLOCK */
  reg.r0.StartClkHS = 1; /* only H Speed clock used */
  reg.r0.CalResN    = 0; /* 2 periods used for calibration */
  reg.r0.ClkHSDiv   = 0;
  reg.r0.Calibrate  = 1; /* calibrated */
  reg.r0.DisAutoCal = 0; /* autocalibration enabled */
  reg.r0.NegStart   = 0;   /* rising */
  reg.r0.NegStop1   = 0;   /* rising */
  reg.r0.NegStop2   = 0;   /* rising */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 0, reg.uint);
	
  r1.uint = TDCGP2_REG1_DEFAULT;
  r1.r1.HitIn1 = 2; /* one stop pulse expected */
  r1.r1.HitIn2 = 0;
  r1.r1.Hit1   = 5;  /* ALU DISABLED */
  r1.r1.Hit2   = 5;  /* ALU DISABLED */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 1, r1.uint);
	
  reg.uint = TDCGP2_REG2_DEFAULT;
  reg.r2.RfEdge1 = 0; /* one edge */
  reg.r2.RfEdge2 = 0; /* one edge */
  XSPI_tdcgp2_RegWrite(InstanceSPI, 2, reg.uint);
  
  reg.uint = TDCGP2_REG3_DEFAULT;  
  reg.r3.EnErrVal = 1; /* timeout error detection */
  reg.r3.DelVal2 = 0;
  XSPI_tdcgp2_RegWrite(InstanceSPI, 3, reg.uint);
  
  reg.uint = TDCGP2_REG4_DEFAULT;  
  XSPI_tdcgp2_RegWrite(InstanceSPI, 4, reg.uint);
  
  reg.uint = TDCGP2_REG5_DEFAULT;  
  XSPI_tdcgp2_RegWrite(InstanceSPI, 5, reg.uint);
  
  return r1;
}

tdcgp2_reg_t XSPI_tdcgp2_MeasCalibration_Configure(XSPI *InstanceSPI)
{
  return  XSPI_tdcgp2_Meas1Calibrated_Configure(InstanceSPI);
}

#undef DBG
