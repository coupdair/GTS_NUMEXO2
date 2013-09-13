#include "xdebug_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include <math.h>
#include <time.h>
#include <unistd.h>

#define DBG XMMR_MUXINOUT_DBG

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

static void external_txmux_set( XMMRegs_fine_delay_ctrl *m, int b )
{
  m->tx_sel0 = b;
  m->tx_sel1 = b;
}

static void external_rxmux_set( XMMRegs_fine_delay_ctrl *m, int b )
{
  m->rx_sel0 = b;
  m->rx_sel1 = b;
}

static void disable_tdc(XMMRegs_tdc_en_ctrl *t)
{
  XMMRegs_tdc_en_ctrl a;

  a = *t;
  a.en_start = 0;
  a.en_stop1 = 0;
  a.en_stop2 = 0;
  *t = a;
}

static void enable_tdc(XMMRegs_tdc_en_ctrl *t)
{
  XMMRegs_tdc_en_ctrl a;

  a = *t;
  a.en_start = 1;
  a.en_stop1 = 1;
  a.en_stop2 = 0; /* not used */
  *t = a;
}

static void set_fine_delay(XMMRegs_fine_delay_ctrl *d, unsigned int fine_delay)
{
  d->LEN0 = 1; /* latch old value, is for ADC_CLK */
  d->LEN1 = 1; /* prepares delayline 1 to receive new data */
  d->delay = fine_delay;
  d->LEN1 = 0; /* passes through fine_delay into delayline 1 */
  d->LEN1 = 1; /* latches the passed value */
}

int translate_from_ps_to_delay(int delay_in_ps)
{
  const int D[11] = {10, 15, 35, 70, 145, 290, 575, 1150, 2300, 4610, 9220};
  int t, i, result[11], total = 0;
  int translated_delay = 0;

  if ( (delay_in_ps < 0) || (delay_in_ps > 18420) ) {
    DBG(DBLE, "ERROR : out of range on delay in function translate_delay_from_ps\n");
  }
  else {
    t = delay_in_ps;
    for (i = 10; i  >= 0; i--) {
      result[i] = floor( ((double) t) / ((double) D[i]) );
      t = t - result[i]*D[i];
      if ( (t < 0) || ( (result[i] != 0) && (result[i] != 1) ) ) {
        DBG(DBLE, "ERROR : something went wrong in function set_delay_in_ps\n") 
      }
    }
    for (i = 0; i <= 10; i++) {
      total += D[i]*result[i];
      translated_delay = set_bit(translated_delay, i, result[i]);
    }
    DBG(DBLI, "The estimated translated delay is : %d ps\n", total);
    DBG(DBLI, "fine_delay = %d", translated_delay);
  }

  return translated_delay;
}

int translate_from_delay_to_ps(unsigned int delay) 
{
  const int D[11] = {10, 15, 35, 70, 145, 290, 575, 1150, 2300, 4610, 9220};
  int d = 0, i, bit;

  if ( delay > 2047 ) {
    DBG(DBLE, "ERROR : out of range on delay in function translate_delay_from_ps\n");
  }
  else {
    for (i = 0; i <= 10; i++) {
      bit = ( ( delay << ( 31-i ) ) >> 31 ); 
      d += bit * D[i];
    }
    DBG(DBLI, "The read delay is : %d ps\n", d);
  }

  return d;
}
/* NUMEXO2 : reset_pll_monitor is not implmented any more
static void reset_pll_monitor(XMMRegs_fine_delay_ctrl *d)
{
  d->rst_pll_monitor = 1;
  d->rst_pll_monitor = 0;
}
*/
static int pll_has_unlocked(XMMRegs_hardware_status *h)
{
//  return (h->pll_has_unlocked); 
  return 0; // NUMEXO2
}

static void init_led(XMMRegs_led_ctrl *l)
{
  l->led1_b       = 0;
  l->led1_g       = 0;
  l->led1_r       = 0;
  l->led0_b       = 0;
  l->led0_g       = 0;
  l->led0_r       = 0;
  l->mux_led      = 0; /* leds are controlled by hardware */
}

static void stop_led(XMMRegs_led_ctrl *l)
{
  l->led1_b       = 0;
  l->led1_g       = 0;
  l->led1_r       = 0;
  l->led0_b       = 0;
  l->led0_g       = 0;
  l->led0_r       = 0;
  l->mux_led      = 0; /* leds are controlled by hardware */
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

void XMMRegs_MuxInOut_Tdc_Enable(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_en_ctrl *t = (XMMRegs_tdc_en_ctrl *)( ba + XMMR_TDC_EN_CTRL_OFFSET );

  enable_tdc(t);
}

void XMMRegs_MuxInOut_Tdc_Disable(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_en_ctrl *t = (XMMRegs_tdc_en_ctrl *)( ba + XMMR_TDC_EN_CTRL_OFFSET );

  disable_tdc(t);
}

void XMMRegs_MuxInOut_ExternalTxMux_Set(XMMRegs *InstancePtr, int b)
{
  external_txmux_set((XMMRegs_fine_delay_ctrl *)(InstancePtr->BaseAddress + XMMR_FINE_DELAY_CTRL_OFFSET),b);
}

void XMMRegs_MuxInOut_ExternalRxMux_Set(XMMRegs *InstancePtr, int b)
{
  external_rxmux_set((XMMRegs_fine_delay_ctrl *)(InstancePtr->BaseAddress + XMMR_FINE_DELAY_CTRL_OFFSET),b);
}

void XMMRegs_MuxInOut_PllMonitor_Reset(XMMRegs *InstancePtr)
{
  XMMRegs_fine_delay_ctrl *c;
  void *ba = InstancePtr->BaseAddress;

  c = (XMMRegs_fine_delay_ctrl *) (ba + XMMR_FINE_DELAY_CTRL_OFFSET);

//  c->rst_pll_monitor = 1; // NUMEXO2 : reset_pll_monitor is not implemented any more
//  c->rst_pll_monitor = 0;
}

int XMMRegs_MuxInOut_IsPllLocked(XMMRegs *InstancePtr)
{
  XMMRegs_fine_delay_ctrl *c;
  XMMRegs_hardware_status *s;
  void *ba = InstancePtr->BaseAddress;
/*
  c = (XMMRegs_fine_delay_ctrl *) (ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  s = (XMMRegs_hardware_status *) (ba + XMMR_HARDWARE_STATUS_OFFSET);

  if (c->rst_pll_monitor == 1) {
    c->rst_pll_monitor = 0;
  }

  microsleep(WAIT_PLL_DUMMY_LOCK);

  if (s->pll_has_unlocked == 0) {
    return PLL_LOCKED;
  }
  else {
    c->rst_pll_monitor = 1;
    c->rst_pll_monitor = 0;
    return PLL_NOT_LOCKED; 
  }
*/
  return PLL_LOCKED; // NUMEXO2
}

int XMMRegs_MuxInOut_FineDelay_Set(XMMRegs *InstancePtr, unsigned int delay)
{
  XMMRegs_fine_delay_ctrl *f;
  void *ba = InstancePtr->BaseAddress;

  if ( delay > MAX_DELAY ) {
    DBG(DBLE, "ERROR : delay is out of range in XMMRegs_MuxInOut_FineDelay_Set\n");
    return XST_FAILURE; 
  }

  f = (XMMRegs_fine_delay_ctrl *) (ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  set_fine_delay(f, delay);

  return XST_SUCCESS;
}

unsigned int XMMRegs_MuxInOut_FromPsToFineDelay_Translate(XMMRegs *InstancePtr, int delay_in_ps)
{
  unsigned int fine_delay;

  fine_delay = (unsigned int) translate_from_ps_to_delay(delay_in_ps);

  return fine_delay;
}

int XMMRegs_MuxInOut_FineDelayInPs_Read(XMMRegs *InstancePtr)
{
  XMMRegs_fine_delay_ctrl *d;
  void *ba = InstancePtr->BaseAddress;
  unsigned int fine_delay;

  d = (XMMRegs_fine_delay_ctrl *) (ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  fine_delay = d->delay;

  return ( translate_from_delay_to_ps(fine_delay) ); 
}

unsigned int XMMRegs_MuxInOut_FineDelay_Read(XMMRegs *InstancePtr)
{
  XMMRegs_fine_delay_ctrl *d;
  void *ba = InstancePtr->BaseAddress;

  d = (XMMRegs_fine_delay_ctrl *) (ba + XMMR_FINE_DELAY_CTRL_OFFSET);

  return (d->delay); 
}

/* At every step_interval, the actual value of the fine_delay is changed by step_size until it reaches
the value delay_after given as argument. 
The user has controlled by other means that delay_before is the actual delay written to the delay line.
step_interval is given in us; if it is 0, the change is fast;
*/
int XMMRegs_MuxInOut_FineDelay_GradualSet(XMMRegs *InstancePtr, unsigned int delay_before, unsigned int delay_after)
{
  int status = XST_SUCCESS;
  unsigned int delay;
  void *ba = InstancePtr->BaseAddress;
  unsigned int step_size, step_interval;
  XMMRegs_fine_delay_ctrl *d = (XMMRegs_fine_delay_ctrl *) (ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  XMMRegs_hardware_status *h = (XMMRegs_hardware_status *) (ba + XMMR_HARDWARE_STATUS_OFFSET);

  step_size = InstancePtr->Settings.fineDelayStepSize;
  step_interval = InstancePtr->Settings.fineDelayStepInterval;

  if ( (delay_before > MAX_DELAY) || (delay_after > MAX_DELAY) ) {
    DBG(DBLE, "ERROR : delay_before or delay_after is out of range in XMMRegs_MuxInOut_FineDelay_GradualSet\n");
    return XST_FAILURE; 
  }

  if (step_size > MAX_DELAY) {
    DBG(DBLE, "ERROR : step_size is out of range in XMMRegs_MuxInOut_FineDelay_GradualSet\n");
    return XST_FAILURE;
  }

  DBG(DBLI, "DelayLine : the actual delay is : %u\n", d->delay);
  DBG(DBLI, "DelayLine : you think it is     : %u\n", (unsigned int)delay_before);
  DBG(DBLD, "DelayLine : step_size = %u, step_interval = %u microsec\n", step_size, step_interval);

//  reset_pll_monitor(d); // NUMEXO2 : reset_pll_monitor is not implemented any more
  delay = delay_before;
  set_fine_delay(d, delay);

  if (step_size != 0) {
    if (delay_after > delay_before) {
      while (delay < delay_after) {
        set_fine_delay(d,delay);
        delay += step_size;
        microsleep(step_interval);
      }
    }
    else {
      while (delay > delay_after) {
        set_fine_delay(d,delay);
        delay -= step_size;
        microsleep(step_interval);
      }
    }
  }

  set_fine_delay(d, delay_after); /* if step_size == 0, only this step is done */
  microsleep(step_interval);

  if ( pll_has_unlocked(h) ) {
    status |= XST_FAILURE;
    DBG(DBLE, "ERROR : the lmk_pll has unlocked during the change of the delay in XMMRegs_MuxInOut_FineDelay_GradualSet\n");
    DBG(DBLE, "you have to decrease the step size or to increase the step_interval\n"); 
  }
  else {
    DBG(DBLI, "DelayLine : gradual setting of the delay was fine\n");
    DBG(DBLI, "DelayLine : the final delay is : %u\n", d->delay); 
  }

//  reset_pll_monitor(d); // NUMEXO2 : reset_pll_monitor is not implemented any more

  return XST_SUCCESS;
}

/************************************************************

	  INIT, RESET and PRINT FUNCTIONS

************************************************************/

void XMMRegs_MuxInOut_Setup(XMMRegs *InstancePtr)
{
  XMMRegs_ctrl *c;
  XMMRegs_fine_delay_ctrl *d;
  void *ba = InstancePtr->BaseAddress;

  c = (XMMRegs_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  set_fine_delay( &(c->fine_delay), FINE_DELAY_DEFAULT );
//  reset_pll_monitor( &(c->fine_delay) ); // NUMEXO2 : reset_pll_monitor is not implemented any more

  d = (XMMRegs_fine_delay_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  external_txmux_set( d, 0 );
  external_rxmux_set( d, 0 );
}

void XMMRegs_MuxInOut_Start(XMMRegs *InstancePtr)
{
}

void XMMRegs_MuxInOut_Stop(XMMRegs *InstancePtr)
{
  XMMRegs_ctrl *c;
  XMMRegs_fine_delay_ctrl *d;
  void *ba = InstancePtr->BaseAddress;

  c = (XMMRegs_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  set_fine_delay( &(c->fine_delay), FINE_DELAY_DEFAULT );
//  reset_pll_monitor( &(c->fine_delay) ); // NUMEXO2 : reset_pll_monitor is not implemented any more

  d = (XMMRegs_fine_delay_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  external_txmux_set( d, 0 );
  external_rxmux_set( d, 0 );
}  

void XMMRegs_MuxInOut_Init(XMMRegs *InstancePtr)
{
  DBG(DBLD, "MuxInOut_Init --->\n");
  XMMRegs_MuxInOut_Setup(InstancePtr);
  XMMRegs_MuxInOut_Start(InstancePtr);
  DBG(DBLD, "---> MuxInOut_Init |\n");
}

void XMMRegs_MuxInOut_Reset(XMMRegs *InstancePtr)
{
  XMMRegs_MuxInOut_Stop(InstancePtr);
  XMMRegs_MuxInOut_Init(InstancePtr);
}

void XMMRegs_MuxInOut_PrintAll(XMMRegs *InstancePtr)
{
  DBG(DBLI, "\nPrintAll of MuxInOut :---------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "fine_delay_ctrl\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_FINE_DELAY_CTRL_OFFSET);
}

#undef DBG
