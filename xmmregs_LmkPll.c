#include "xdebug_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xmmregs_LmkPll.h"
#include "xmmregs_LmkPll_i.h"

#define DBG XMMR_LMKPLL_DBG

/********************************************************************************

		LOADS reg data to LMK register

********************************************************************************/

int XMMRegs_LmkPll_WriteReg(XMMRegs *InstancePtr, unsigned int wdata)
{
  unsigned int bit;

  XMMRegs_LmkPll_HW_L(InstancePtr->BaseAddress);
  XMMRegs_LmkPll_LE_L(InstancePtr->BaseAddress);
  DBG(DBLD, "write to external PLL : 0x%08X\n", wdata);

  for(bit=0x80000000;bit;bit = bit >> 1)
  {
    if(wdata & bit)
      XMMRegs_LmkPll_DATA_H(InstancePtr->BaseAddress);
    else
      XMMRegs_LmkPll_DATA_L(InstancePtr->BaseAddress);
    XMMRegs_LmkPll_CLK_H(InstancePtr->BaseAddress);
    XMMRegs_LmkPll_CLK_L(InstancePtr->BaseAddress);
  }
  XMMRegs_LmkPll_LE_H(InstancePtr->BaseAddress);
  XMMRegs_LmkPll_LE_L(InstancePtr->BaseAddress);

  return XST_SUCCESS;
}

int XMMRegs_LmkPll_LoadRegConfig(XMMRegs *InstancePtr, lmk_config_reg_t *c, int n)
{
  int i;
  int status = XST_SUCCESS;

  for(i=0;i<n;i++) {
    status |= XMMRegs_LmkPll_WriteReg(InstancePtr, c[i].binary);
    if (c[i].r1r7.address == 4) {
      status |= XMMRegs_LmkPll_FineDelay_LogSet(InstancePtr, c[i].r1r7.CLKout_DLY);
    }
  }

  return status;
}

int XMMRegs_LmkPll_LoadRegConfig_Default(XMMRegs *InstancePtr) {
//  int n = sizeof(lmk_config_reg_default)/sizeof(lmk_config_reg_t);
  int n = 12;
  return XMMRegs_LmkPll_LoadRegConfig(InstancePtr, lmk_config_reg_default, n);
}

unsigned int XMMRegs_LmkPll_FineDelay_Read(XMMRegs *InstancePtr) {
  lmk_config_reg_t *c;
  c = (lmk_config_reg_t *) (&InstancePtr->Data.lmk_config_CLKout4);
  return c->r1r7.CLKout_DLY;
}

/* it updates the value of delay in XMMRegsdriver.Data without loading it in the LmkPll 
it is useful as one can't read back from the LmkPll registers */
int XMMRegs_LmkPll_FineDelay_LogSet(XMMRegs *InstancePtr, unsigned int delay) {
  lmk_config_reg_t *c;
  c = (lmk_config_reg_t *) (&InstancePtr->Data.lmk_config_CLKout4);

  if (delay > 0xF) {
    DBG(DBLE, "ERROR : out of range on delay (should be less than 0xF) in XMMRegs_LmkPll_FineDelay_Set");
    return XST_FAILURE;
  }

  c->r1r7.CLKout_DLY = delay;

  return XST_SUCCESS;
}

int XMMRegs_LmkPll_FineDelay_Set(XMMRegs *InstancePtr, unsigned int delay) {
  lmk_config_reg_t *c;
  c = (lmk_config_reg_t *) (&InstancePtr->Data.lmk_config_CLKout4);

  if (delay > 0xF) {
    DBG(DBLE, "ERROR : out of range on delay (should be less than 0xF) in XMMRegs_LmkPll_FineDelay_Set");
    return XST_FAILURE;
  }

  c->r1r7.CLKout_DLY = delay;

  return XMMRegs_LmkPll_LoadRegConfig(InstancePtr, c, 1);
}

/* At every step_interval, the actual value of the LmkPll_fine_delay is changed by step_size until it reaches
the value delay_after given as argument. 
The user has controlled by other means that delay_before is the actual delay written to the delay line.
step_interval is given in ns; if it's 0, the change is fast;
*/
int XMMRegs_LmkPll_FineDelay_GradualSet(XMMRegs *InstancePtr, unsigned int old_delay, unsigned int new_delay) {
  int status = XST_SUCCESS;
  unsigned int step_size, step_interval;
  unsigned int delay;

  step_size = InstancePtr->Settings.LmkPllDelayStepSize;
  step_interval = InstancePtr->Settings.LmkPllDelayStepInterval;

  if ( (old_delay > MAX_LMKPLL_DELAY) || (new_delay > MAX_LMKPLL_DELAY) ) {
    DBG(DBLE, "ERROR : old_delay or new_delay is out of range in XMMRegs_LmkPll_FineDelay_GradualSet\n");
    return XST_FAILURE; 
  }
  if (step_size > MAX_LMKPLL_DELAY) {
    DBG(DBLE, "ERROR : step_size is out of range in XMMRegs_LmkPll_FineDelay_GradualSet\n");
    return XST_FAILURE;
  }

  DBG(DBLI, "LmkPll : the actual delay is : %u\n", XMMRegs_LmkPll_FineDelay_Read(InstancePtr) );
  DBG(DBLI, "LmkPll : you think it is     : %u\n", old_delay);
  DBG(DBLD, "LmkPll : step_size = %u, step_interval = %u microsec\n", step_size, step_interval);

  status |= XMMRegs_MuxInOut_PllMonitor_Reset(InstancePtr);
  delay = old_delay;
  status |= XMMRegs_LmkPll_FineDelay_Set(InstancePtr, delay);

  if (step_size != 0) {
    if (new_delay > old_delay) {
      while (delay < new_delay) {
        status |= XMMRegs_LmkPll_FineDelay_Set(InstancePtr, delay);
        delay += step_size;
        microsleep(step_interval);
      }
    }
    else {
      while (delay > new_delay) {
        status |= XMMRegs_LmkPll_FineDelay_Set(InstancePtr, delay);
        delay -= step_size;
        microsleep(step_interval);
      }
    }
  }

  status |= XMMRegs_LmkPll_FineDelay_Set(InstancePtr, delay); /* if step_size == 0, only this step is done */
  microsleep(step_interval);

  if ( XMMRegs_MuxInOut_IsPllLocked(InstancePtr) != PLL_LOCKED ) {
    status |= XST_FAILURE;
    DBG(DBLE, "ERROR : the lmk_pll has unlocked during the change of the delay\n");
    DBG(DBLE, "you have to decrease the step size or to increase the step interval\n"); 
  }
  else {
    DBG(DBLI, "LmkPll : gradual setting of the delay was fine\n");
    DBG(DBLI, "LmkPll : the final delay is : %u\n", XMMRegs_LmkPll_FineDelay_Read(InstancePtr) ); 
  }

  status |= XMMRegs_MuxInOut_PllMonitor_Reset(InstancePtr); 

  return status;
}

int XMMRegs_LmkPll_LoadRegConfig_Change(XMMRegs *InstancePtr, int PLL_R, int PLL_N, int VCO_DIV) {
  int status = XST_SUCCESS;
//  int n = sizeof(lmk_config_reg_change)/sizeof(lmk_config_reg_t);
  int n = 12;

  lmk_config_reg_change[n-2].r14.PLL_R = PLL_R;
  lmk_config_reg_change[n-1].r15.PLL_N = PLL_N;
  lmk_config_reg_change[n-1].r15.VCO_DIV = VCO_DIV;
  status |= XMMRegs_LmkPll_LoadRegConfig(InstancePtr, lmk_config_reg_change, n);

  return status;
}

int XMMRegs_LmkPll_ConfigReset(XMMRegs *InstancePtr) {
//  int n = sizeof(lmk_config_reg_reset)/sizeof(lmk_config_reg_t);
  int n = 1;
  return XMMRegs_LmkPll_LoadRegConfig(InstancePtr, lmk_config_reg_reset, n);
}

/*******************************************************************************

		LOADS raw data to LMK register

*******************************************************************************/

/*************************************************
* Description : Writes data to a LMK register
* Parameters:
*   InstancePtr : driver
*   address     : the register address
*   data        : data to write 
* Return value:
*   int         : status
**************************************************/
int XMMRegs_LmkPll_WriteRaw(XMMRegs *InstancePtr, unsigned int address, unsigned int data)
{
  unsigned int wdata,bit;

  if ( (data > 0xFFFFFFF) || (address > 0xF) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_LmkPll_WriteRaw");
    return XST_FAILURE;
  }

  wdata = (data << 4) | (address & 0x0F);
  XMMRegs_LmkPll_HW_L(InstancePtr->BaseAddress);
  XMMRegs_LmkPll_LE_L(InstancePtr->BaseAddress);
  for(bit=0x80000000;bit;bit = bit >> 1)
  {
    if(wdata & bit)
      XMMRegs_LmkPll_DATA_H(InstancePtr->BaseAddress);
    else
      XMMRegs_LmkPll_DATA_L(InstancePtr->BaseAddress);
    XMMRegs_LmkPll_CLK_H(InstancePtr->BaseAddress);
    XMMRegs_LmkPll_CLK_L(InstancePtr->BaseAddress);
  }
  XMMRegs_LmkPll_LE_H(InstancePtr->BaseAddress);
  XMMRegs_LmkPll_LE_L(InstancePtr->BaseAddress);

  return XST_SUCCESS;
}

/**************************************************
*Description : Loads raw config
*Parameters:
*   InstancePtr : driver
*Return Value:
*   int : status
***************************************************/
int XMMRegs_LmkPll_LoadRawConfig(XMMRegs *InstancePtr, lmk_config_raw_t *c, int n)
{
  int i;
  int status = XST_SUCCESS;

  for(i=0;i<n;i++)
    status |= XMMRegs_LmkPll_WriteRaw(InstancePtr, c[i].address, c[i].data);

  return status;
}

/* loads default raw config */
int XMMRegs_LmkPll_LoadRawConfig_Default(XMMRegs *InstancePtr)
{
//  int n = sizeof(lmk_config_raw_default)/sizeof(lmk_config_raw_t);
  int n = 12;
  return XMMRegs_LmkPll_LoadRawConfig(InstancePtr, lmk_config_raw_default, n);
}

/*******************************************************************************

		Other LMK PLL functions

*******************************************************************************/

/* Description : Check if PLL is locked */
int XMMRegs_LmkPll_IsLocked(XMMRegs *InstancePtr)
{
  return (int) XMMRegs_LmkPll_LOCKED(InstancePtr->BaseAddress);
}

/* Description : Disables Global Output of the pll */
void XMMRegs_LmkPll_GlobalOutputDisable(XMMRegs *InstancePtr)
{
  XMMRegs_LmkPll_GOE_L(InstancePtr->BaseAddress);
}

/* Description : Enables Global Output of the pll */
void XMMRegs_LmkPll_GlobalOutputEnable(XMMRegs *InstancePtr)
{
  XMMRegs_LmkPll_GOE_H(InstancePtr->BaseAddress);
}


/*******************************************************************************

	  INIT, RESET and PRINT FUNCTIONS

*******************************************************************************/

int XMMRegs_LmkPll_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  int i;
//  int n = sizeof(lmk_config_reg_default)/sizeof(lmk_config_reg_t);
  int n = 12;

  lmk_config_reg_t *cd = (lmk_config_reg_t *) (&InstancePtr->Data.lmk_config_CLKout4);
  XMMRegs_lmk_pll_ctrl *c = (XMMRegs_lmk_pll_ctrl *)(InstancePtr->BaseAddress + XMMR_LMK_PLL_CTRL_OFFSET);

  status |= XMMRegs_LmkPll_LoadRegConfig_Default(InstancePtr);

  c->data      = 0;
  c->clk       = 0;
  c->le        = 0;
  c->goe       = 1;
  c->sync_pll  = 0;
  c->sync_pll  = 1; /* synchronizes the enables outputs */

  for (i = 0; i < n; i++) {
    if (lmk_config_reg_default[i].r1r7.address == 4) {
      if (lmk_config_reg_default[i].binary != cd->binary) {
        DBG(DBLW, "WARNING : lmk_config_CLKout4 is not equal to its expected default value in XMMRegs_LmkPll_Setup\n");
      } 
      cd->binary = lmk_config_reg_default[i].binary;
    }
  }

  return status;
}

/* redundant with setup */
int XMMRegs_LmkPll_Start(XMMRegs *InstancePtr)
{
  XMMRegs_lmk_pll_ctrl *c = (XMMRegs_lmk_pll_ctrl *)(InstancePtr->BaseAddress + XMMR_LMK_PLL_CTRL_OFFSET);

  c->data      = 0;
  c->clk       = 0;
  c->le        = 0;
  c->goe       = 1;
  c->sync_pll  = 1; 

  return XST_SUCCESS;
}

int XMMRegs_LmkPll_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  int i;
//  int n = sizeof(lmk_config_reg_default)/sizeof(lmk_config_reg_t);
  int n = 12;

  lmk_config_reg_t *cd = (lmk_config_reg_t *) (&InstancePtr->Data.lmk_config_CLKout4);
  XMMRegs_lmk_pll_ctrl *c = (XMMRegs_lmk_pll_ctrl *)(InstancePtr->BaseAddress + XMMR_LMK_PLL_CTRL_OFFSET);

  c->data      = 0;
  c->clk       = 0;
  c->le        = 0;
  c->goe       = 0;
  c->sync_pll  = 0; 

  status = XMMRegs_LmkPll_ConfigReset(InstancePtr);

  for (i = 0; i < n; i++) {
    if (lmk_config_reg_default[i].r1r7.address == 4) {
      cd->binary = lmk_config_reg_default[i].binary;
    }
  }

  return status;
}  

int XMMRegs_LmkPll_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_LmkPll_Setup(InstancePtr);
  status |= XMMRegs_LmkPll_Start(InstancePtr);

  return status;
}

int XMMRegs_LmkPll_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_LmkPll_Stop(InstancePtr);
  status |= XMMRegs_LmkPll_Init(InstancePtr);

  return status;
}

void XMMRegs_LmkPll_PrintAll(XMMRegs *InstancePtr)
{
  DBG(DBLI, "\nPrintAll of LmkPll :---------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "lmk_pll_ctrl\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_LMK_PLL_CTRL_OFFSET);
  DBG(DBLI, "lmk_pll_status\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_LMK_PLL_STATUS_OFFSET);
}

#undef DBG
