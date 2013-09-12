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

static void ctrl_to_software_for_external_mux(XMMRegs_mgt_sel_ctrl *m)
{
  m->ctrl_enable = 1;
  return; 
}

static void ctrl_to_hardware_for_external_mux(XMMRegs_mgt_sel_ctrl *m)
{
  m->ctrl_enable = 0;
  return; 
}

static int external_txmux_set( XMMRegs_fine_delay_ctrl *m, int transceiver, int mgt_sfp )
{
  int status = XST_SUCCESS;

  switch (mgt_sfp) {
    case USE_MGT :
      m->tx_sel0 = 0;
      m->tx_sel1 = 0;
      break;
    case BYPASS_MGT :
      m->tx_sel0 = 1;
      m->tx_sel1 = 1;
      break;
    default :
      status = XST_FAILURE;
      break; 
  }

  return status;
}

static int external_rxmux_set( XMMRegs_fine_delay_ctrl *m, int transceiver, int mgt_sfp )
{
  int status = XST_SUCCESS;

  switch (mgt_sfp) {
    case USE_MGT : 
      m->rx_sel0 = 0;
      m->rx_sel1 = 0;
      break;
    case BYPASS_MGT :
      m->rx_sel0 = 1;
      m->rx_sel1 = 1;
      break;
    default :
      status = XST_FAILURE;
      break; 
  }

  return status;
}

static void disable_tdc(XMMRegs_tdc_en_ctrl *t)
{
  XMMRegs_tdc_en_ctrl a;

  a = *t;
  a.en_start = 0;
  a.en_stop1 = 0;
  a.en_stop2 = 0;
  *t = a;
  return; 
}

static void enable_tdc(XMMRegs_tdc_en_ctrl *t)
{
  XMMRegs_tdc_en_ctrl a;

  a = *t;
  a.en_start = 1;
  a.en_stop1 = 1;
  a.en_stop2 = 0; /* not used */
  *t = a;
  return; 
}

static void set_fine_delay(XMMRegs_fine_delay_ctrl *d, unsigned int fine_delay)
{
  d->LEN0 = 1; /* latch old value, is for ADC_CLK */
  d->LEN1 = 1; /* prepares delayline 1 to receive new data */
  d->delay = fine_delay;
  d->LEN1 = 0; /* passes through fine_delay into delayline 1 */
  d->LEN1 = 1; /* latches the passed value */
  return;
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
  return;
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

  return;
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

  return;
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

int XMMRegs_MuxInOut_TriggerLed_Set(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_led_ctrl *t = (XMMRegs_led_ctrl *) ( ba + XMMR_LED_CTRL_OFFSET );

  t->mux_led = 1;

  return XST_SUCCESS;
}

int XMMRegs_MuxInOut_DefaultLed_Set(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_led_ctrl *t = (XMMRegs_led_ctrl *) ( ba + XMMR_LED_CTRL_OFFSET );

  t->mux_led = 0;

  return XST_SUCCESS;
}

int XMMRegs_MuxInOut_Tdc_Enable(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_en_ctrl *t = (XMMRegs_tdc_en_ctrl *) ( ba + XMMR_TDC_EN_CTRL_OFFSET );

  enable_tdc(t);

  return XST_SUCCESS;
}

int XMMRegs_MuxInOut_Tdc_Disable(XMMRegs *InstancePtr)
{
  void *ba = InstancePtr->BaseAddress;
  XMMRegs_tdc_en_ctrl *t = (XMMRegs_tdc_en_ctrl *) ( ba + XMMR_TDC_EN_CTRL_OFFSET );

  disable_tdc(t);

  return XST_SUCCESS;
}

int XMMRegs_MuxInOut_ExternalTxMux_Set(XMMRegs *InstancePtr, int transceiver, int mgt_sfp)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_MuxInOut_ExternalTxMux_Set\n");
    status = XST_FAILURE;
  }
  else {
    status = external_txmux_set((XMMRegs_fine_delay_ctrl *)(InstancePtr->BaseAddress + XMMR_FINE_DELAY_CTRL_OFFSET),transceiver,mgt_sfp);
  }

  return status;
}

int XMMRegs_MuxInOut_ExternalRxMux_Set(XMMRegs *InstancePtr, int transceiver, int mgt_sfp)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_MuxInOut_ExternalTxMux_Set\n");
    status = XST_FAILURE;
  }
  else {
    status = external_rxmux_set((XMMRegs_fine_delay_ctrl *)(InstancePtr->BaseAddress + XMMR_FINE_DELAY_CTRL_OFFSET),transceiver,mgt_sfp );
  }

  return status;
}

int XMMRegs_MuxInOut_PllMonitor_Reset(XMMRegs *InstancePtr)
{
  XMMRegs_fine_delay_ctrl *c;
  void *ba = InstancePtr->BaseAddress;

  c = (XMMRegs_fine_delay_ctrl *) (ba + XMMR_FINE_DELAY_CTRL_OFFSET);

//  c->rst_pll_monitor = 1; // NUMEXO2 : reset_pll_monitor is not implemented any more
//  c->rst_pll_monitor = 0;

  return XST_SUCCESS;
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

int XMMRegs_MuxInOut_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_ctrl *c;
  XMMRegs_fine_delay_ctrl *d;
  void *ba = InstancePtr->BaseAddress;

  c = (XMMRegs_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  set_fine_delay( &(c->fine_delay), FINE_DELAY_DEFAULT );
//  reset_pll_monitor( &(c->fine_delay) ); // NUMEXO2 : reset_pll_monitor is not implemented any more

  d = (XMMRegs_fine_delay_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  external_txmux_set( d, 0, USE_MGT );
  external_rxmux_set( d, 0, USE_MGT );

  if ( XMMRegs_IsTransceiverConnected(InstancePtr, 0) == TRANSCEIVER_IS_CONNECTED) 
  {
    if ( XMMRegs_IsTransceiverInGtsTree(InstancePtr, 0) == TRANSCEIVER_IS_IN_GTS_TREE )
    {
      external_txmux_set( d, 0, USE_MGT );
      external_rxmux_set( d, 0, USE_MGT );
    }
    else if ( XMMRegs_IsTransceiverConnectedToDigitizer(InstancePtr, 0) == TRANSCEIVER_IS_CONNECTED_TO_DIGITIZER) 
    { /* the RocketIO is bypassed */
      external_txmux_set( d, 0, BYPASS_MGT );
      external_rxmux_set( d, 0, BYPASS_MGT );
    }
    else { /* it is connected neither to the GTS tree nor to the digitizer */
      external_txmux_set( d, 0, USE_MGT );
      external_rxmux_set( d, 0, USE_MGT );
    }
  }
  else { /* the transceiver is not connected */
    /* it is thus bypassed */
    external_txmux_set( d, 0, BYPASS_MGT );
    external_rxmux_set( d, 0, BYPASS_MGT );
  }
  
//  ctrl_to_software_for_external_mux(mgt_sel);

  return status;
}

int  XMMRegs_MuxInOut_Start(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  return status;
}

int XMMRegs_MuxInOut_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_ctrl *c;
  XMMRegs_fine_delay_ctrl *d;
  void *ba = InstancePtr->BaseAddress;

  c = (XMMRegs_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
  set_fine_delay( &(c->fine_delay), FINE_DELAY_DEFAULT );
//  reset_pll_monitor( &(c->fine_delay) ); // NUMEXO2 : reset_pll_monitor is not implemented any more

  d = (XMMRegs_fine_delay_ctrl *)(ba + XMMR_FINE_DELAY_CTRL_OFFSET);
//  ctrl_to_hardware_for_external_mux(mgt_sel);
  external_txmux_set( d, 0, BYPASS_MGT );
  external_rxmux_set( d, 0, BYPASS_MGT );

  return status;
}  

int XMMRegs_MuxInOut_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  DBG(DBLD, "MuxInOut_Init --->\n");
  status |= XMMRegs_MuxInOut_Setup(InstancePtr);
  status |= XMMRegs_MuxInOut_Start(InstancePtr);
  DBG(DBLD, "---> MuxInOut_Init |\n");

  return status;
}

int XMMRegs_MuxInOut_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_MuxInOut_Stop(InstancePtr);
  status |= XMMRegs_MuxInOut_Init(InstancePtr);

  return status;
}

void XMMRegs_MuxInOut_PrintAll(XMMRegs *InstancePtr)
{
  DBG(DBLI, "\nPrintAll of MuxInOut :---------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");

  DBG(DBLI, "fine_delay_ctrl\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_FINE_DELAY_CTRL_OFFSET);
}

#undef DBG
