#include "align.h"
#include "xmmregs_i.h"
#include "debug.h"

#define DBG DBG_ALIGN_DELAY

/* step_interval is given in microseconds */
int fineDelayParameterSet(unsigned int step_size, unsigned int step_interval)
{
  int status = XST_SUCCESS;
  
  status |= XMMRegs_FineDelayParameter_Set(&XMMRegsDriver, step_size, step_interval);
  
  return status;
}

/* the delay is expressed in units of steps of DelayLine */
int fineDelaySet(unsigned int total_delay)
{
  int status = XST_SUCCESS;
  unsigned int old_delay_LmkPll;
  unsigned int old_delay_DelayLine;
  unsigned int old_total_delay;
  unsigned int new_delay_LmkPll;
  unsigned int new_delay_DelayLine;
  unsigned int new_total_delay;
  
  if (total_delay > MAX_RAW_DELAY_V3) {
    DBG(DBLE, "ERROR : out of range on total_delay in funtion fineDelaySet\n");
    DBG(DBLE, "fine_delay < %d\n", MAX_RAW_DELAY_V3);
    return XST_FAILURE;
  }

//   /************ reading of the old delay values ********************************/
// 
//   old_delay_DelayLine = XMMRegs_MuxInOut_FineDelay_Read(&XMMRegsDriver);
//   old_delay_LmkPll    = XMMRegs_LmkPll_FineDelay_Read(&XMMRegsDriver);
//   old_total_delay     = XMMRegs_Data_TotalDelay_Read(&XMMRegsDriver);
// 
//   /************ checking of the old delay values *******************************/
// 
//   if ( (old_delay_LmkPll != 0) && (old_delay_LmkPll != FIXED_LMKPLL_DELAY) ) {
//     DBG(DBLE, "ERROR : old_delay_LmkPll can have only two values 0 or %d\n", FIXED_LMKPLL_DELAY);
//     DBG(DBLE, "whereas old_delay_LmkPll = %u\n", old_delay_LmkPll);
//     return XST_FAILURE;
//   }
// 
//   if ( old_delay_DelayLine > MAX_DELAY ) {
//     DBG(DBLE, "ERROR : out of range on old_delay_DelayLine in fineDelaySet, should be smaller than %d\n", MAX_DELAY);
//     return XST_FAILURE;
//   } 
// 
//   if ( old_total_delay > MAX_RAW_DELAY_V3 ) {
//     DBG(DBLE, "ERROR : out of range on old_total_delay in fineDelaySet, should be smaller than %d\n", MAX_RAW_DELAY_V3);
//     return XST_FAILURE;
//   } 
// 
//   /************ computation of the new delay values *****************************/
// 
//   if ( total_delay > LIM_SUP_DELAYLINE_LMKPLL ) {
//     if ( old_delay_LmkPll == 0 ) {
//       new_delay_LmkPll = FIXED_LMKPLL_DELAY;
//       new_delay_DelayLine = old_delay_DelayLine + total_delay - old_total_delay - FIXED_LMKPLL_DELAY_IN_DELAYLINE_INCREMENT;
//     }
//     else if ( old_delay_LmkPll == FIXED_LMKPLL_DELAY ) {
//       new_delay_LmkPll = FIXED_LMKPLL_DELAY;
//       new_delay_DelayLine = old_delay_DelayLine + total_delay - old_total_delay;
//     }
//     else {
//       DBG(DBLE, "ERROR : in function fineDelaySet\n");
//       return XST_FAILURE;
//     }
//   }
//   else if (total_delay < LIM_INF_DELAYLINE_LMKPLL) {
//     if ( old_delay_LmkPll == 0 ) {
//       new_delay_LmkPll = 0;
//       new_delay_DelayLine = old_delay_DelayLine + total_delay - old_total_delay;
//     }
//     else if ( old_delay_LmkPll == FIXED_LMKPLL_DELAY ) {
//       new_delay_LmkPll = 0;
//       new_delay_DelayLine = old_delay_DelayLine + total_delay - old_total_delay + FIXED_LMKPLL_DELAY_IN_DELAYLINE_INCREMENT;
//     }
//     else {
//       DBG(DBLE, "ERROR : in function fineDelaySet\n");
//       return XST_FAILURE;
//     }
//   }
//   else {
//     new_delay_LmkPll = old_delay_LmkPll;
//     new_delay_DelayLine = old_delay_DelayLine + total_delay - old_total_delay;
//   }
// 
//   new_total_delay = total_delay;
//   DBG(DBLI, "TotalDelay : old_total_delay = %u\n", old_total_delay);
//   DBG(DBLI, "TotalDelay : new_total_delay = %u\n", new_total_delay);
// 
//   /************ assignment of the new delay values *****************************/
// 
//   if ( old_delay_LmkPll != new_delay_LmkPll ) {   
//     status |= XMMRegs_LmkPll_FineDelay_GradualSet(&XMMRegsDriver, old_delay_LmkPll, new_delay_LmkPll);
//   }
//   if ( old_delay_DelayLine != new_delay_DelayLine ) {
//     status |= XMMRegs_MuxInOut_FineDelay_GradualSet(&XMMRegsDriver, old_delay_DelayLine, new_delay_DelayLine);
//   }

  status |= XMMRegs_MuxInOut_FineDelay_Set(&XMMRegsDriver, total_delay);

  /************ Logging the new delay values ***********************************/
  
  status |= XMMRegs_Data_TotalDelay_Set(&XMMRegsDriver, total_delay);

  return status;
}

int fineDelaySetNow(unsigned int fine_delay)
{
  int status = XST_SUCCESS;
  
  DBG ( DBLI, "old fine delay : %u --> ", XMMRegs_MuxInOut_FineDelay_Read(&XMMRegsDriver) );
  status |= XMMRegs_MuxInOut_FineDelay_Set(&XMMRegsDriver, fine_delay);
  DBG ( DBLI, "new fine delay : %u", XMMRegs_MuxInOut_FineDelay_Read(&XMMRegsDriver) );
  
  return status;
}

int coarseDelaySet(unsigned int coarse_delay)
{
  int status = XST_SUCCESS;
  
  DBG(DBLD, "old coarse delay = %u\n", XMMRegs_DataPath_CoarseDelay_Read(&XMMRegsDriver) );
  status |= XMMRegs_DataPath_CoarseDelay_Set(&XMMRegsDriver, coarse_delay);
  DBG(DBLD, "new coarse delay = %u\n", XMMRegs_DataPath_CoarseDelay_Read(&XMMRegsDriver) );
  
  return status;
}

int fineDelaySwing(int max_iter)
{
  int status = XST_SUCCESS;
  int i = 0;
  unsigned int min_d = 0;
  unsigned int max_d = MAX_DELAY;
  unsigned int old_fine_delay, new_fine_delay;
  
  old_fine_delay = XMMRegs_MuxInOut_FineDelay_Read(&XMMRegsDriver);
  new_fine_delay = max_d;
  
  while (i <= max_iter) 
  {
    status |= XMMRegs_MuxInOut_FineDelay_GradualSet(&XMMRegsDriver, old_fine_delay, new_fine_delay);
    if (new_fine_delay == max_d) {
      new_fine_delay = min_d;
      old_fine_delay = max_d; 
    }
    else {
      new_fine_delay = max_d;
      old_fine_delay = min_d;
    }
    i++;
  }
  
  return status;
}

#undef DBG
