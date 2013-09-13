#include <math.h>
#include "xdebug_l.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xmmregs_RocketIO.h"

#define DBG XMMR_ROCKETIO_DBG

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

static void print_u16_binary(unsigned short *ba)
{
  int i;
  unsigned short val;

  for (i = 0; i < 16; i++)
  {
    if ( ( !(i%4) ) && i ) DBG(DBLD, " : ");

    val = (*ba) << i;

    val = val >> 15;

    DBG(DBLD, "%d", (int)val );
  }

  DBG(DBLD, "\n");
}

static void set_regdata(XMMRegs_txmgtdata_ctrl *reg, unsigned int msb, unsigned int lsb, unsigned char comma_msb, unsigned char comma_lsb)
{
  /* order matters */

  if (comma_msb == 0) {
    reg->txcharisk_msb = 0;
    reg->msb = msb;
  }
  else {
    reg->msb = msb;
    reg->txcharisk_msb = 1;
  }

  if (comma_lsb == 0) {
    reg->txcharisk_lsb = 0;
    reg->lsb = lsb;
  }
  else {
    reg->lsb = lsb;
    reg->txcharisk_lsb = 1;
  }
}

static void setup_gt(XMMRegs_gt_ctrl *gt)
{
  gt->gtx_reset = 1;
  gt->gtsStreamReady = 1;
//  gt->rx_system_reset = 1;
//  gt->tx_system_reset = 1;
  gt->backpressure = 0;
  gt->trigger_rst_carrier = 1;
}

static void stop_gt(XMMRegs_gt_ctrl *gt)
{
  gt->gtx_reset = 1;
  gt->gtsStreamReady = 1;
//  gt->rx_system_reset = 1;
//  gt->tx_system_reset = 1;
  gt->backpressure = 0;
  gt->trigger_rst_carrier = 1;
}

static void init_gt(XMMRegs_gt_ctrl *gt)
{
  gt->gtx_reset = 0;
}

static int status_gt_rx(XMMRegs_gt_status *gt_status)
{
  int status;

  status = gt_status->rx_init_status;

  switch (status) 
  {
    case C_RESET     : case C_PMA_RESET : case C_WAIT_LOCK    :
    case C_PCS_RESET : case C_WAIT_PCS  : case C_ALMOST_READY :
    case C_READY     :
      break;
    default :
      status = C_NULL;
      break;
  }

//  return status;
  return C_READY; // GTS_LEAF on NUMEXO2
}

static int status_gt_tx(XMMRegs_gt_status *gt_status)
{
  int status;

  status = gt_status->tx_init_status;

  switch (status) 
  {
    case C_RESET     : case C_PMA_RESET : case C_WAIT_LOCK    :
    case C_PCS_RESET : case C_WAIT_PCS  : case C_ALMOST_READY :
    case C_READY     :
      break;
    default :
      status = C_NULL;
      break;
  }

//  return status;
  return C_READY; // GTS_LEAF on NUMEXO2
}

static void reset_gt(XMMRegs_gt_ctrl *gt)
{
  gt->gtx_reset = 1;
}

static void init_drp_addr(XMMRegs_drp_addr_ctrl *drp_addr)
{
  drp_addr->drp_choice = 0;
  drp_addr->drp_addr = 0;
}

static void init_drp(XMMRegs_drp_ctrl *drp)
{
  drp->signal_detect = 0xF;
  drp->RST_drp_bsy = 0;
  drp->drp_we = 0;
  drp->drp_en = 0;
  drp->drp_di = 0;
}

static void stop_drp(XMMRegs_drp_ctrl *drp)
{
  drp->signal_detect = 0;
  drp->RST_drp_bsy = 0;
  drp->drp_we = 0;
  drp->drp_en = 0;
  drp->drp_di = 0;
}

static void set_word_drp(XMMRegs_drp_addr_ctrl *drp_addr, XMMRegs_drp_ctrl *drp, XMMRegs_drp_status *drp_status, unsigned char DADDR, unsigned short DI)
{
  drp_addr->drp_addr = DADDR;
  drp->drp_di = DI;
  drp->drp_en = 0;
  drp->drp_we = 1;
  drp->drp_en = 1;
  drp->drp_en = 0;
  drp->drp_we = 1;

  while (drp_status->drp_bsy == 1) ;
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

/* looks if there is a comma alignment */
int XMMRegs_RocketIO_CommaAlign_Diagnose(XMMRegs *InstancePtr)
{
  XMMRegs_gt_status *s;
  void *ba;

  ba = InstancePtr->BaseAddress;
  s = (XMMRegs_gt_status *)(ba + XMMR_GT_STATUS_OFFSET);

  if ( XMMRegs_RocketIO_RxSystem_Status(InstancePtr, 0) == READY )
  {
// F. Saillant le 30 novembre 2012 : on autorise aussi COMMA (0x00BC) car c'est ce que les mezzanines GTS v3 envoient ...

//    if ((s->rxcommadet == 1) && (s->rxchariscomma == 1) && ((XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, transceiver) == ALIGN_WORD) || (XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, transceiver) == COMMA)))
    if ((XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, 0) == ALIGN_WORD) || (XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, 0) == COMMA))
    {
      return COMMA_DETECTED;
    }
  }

  return COMMA_NOT_DETECTED;
}

unsigned int XMMRegs_RocketIO_RxMgtdata_Read(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_rxmgtdata_status *s;
  unsigned int val;

  s = (XMMRegs_rxmgtdata_status *)(InstancePtr->BaseAddress + XMMR_GT_RXDATA_OFFSET + (unsigned int)(4 * floor(transceiver/2)));

  if ( (transceiver%2) == 0)
    val = s->t0; /* one takes the 16 LSB bits */
  else 
    val = s->t1; /* the 16 MSB bits */

  DBG(DBLD, "RXDATA(transceiver %d) = 0x%04X\n", transceiver, val);

  return val;
}

void XMMRegs_RocketIO_RefClk_Set(XMMRegs *InstancePtr)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  void *ba;

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  set_word_drp(drp_addr, drp, drp_status, 0x04, 0xA0E9); // for GTS LEAF NUMEXO2 (Virtex5)
}

/************************************************************

	Lowlevel INIT and RESET FUNCTIONS

************************************************************/

void XMMRegs_RocketIO_TxSystem_Stop(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt;

  gt = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress +  XMMR_GT_CTRL_OFFSET);

  reset_gt(gt);
}

int XMMRegs_RocketIO_RxSystem_Stop(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt;

  gt = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress +  XMMR_GT_CTRL_OFFSET);

  reset_gt(gt);
}

void XMMRegs_RocketIO_TxSystem_Init(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt;

  gt = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  init_gt(gt);
}

void XMMRegs_RocketIO_RxSystem_Init(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt;

  gt = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  init_gt(gt);
}

void XMMRegs_RocketIO_TxSystem_Reset(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_RocketIO_TxSystem_Stop(InstancePtr, transceiver);
  XMMRegs_RocketIO_TxSystem_Init(InstancePtr, transceiver);
}

void XMMRegs_RocketIO_RxSystem_Reset(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_RocketIO_RxSystem_Stop(InstancePtr, transceiver);
  XMMRegs_RocketIO_RxSystem_Init(InstancePtr, transceiver);
}

int XMMRegs_RocketIO_TxSystem_Status(XMMRegs *InstancePtr, int transceiver)
{
  int system_status;
  XMMRegs_gt_status *gt_status;

  gt_status = (XMMRegs_gt_status *)(InstancePtr->BaseAddress + XMMR_GT_STATUS_OFFSET);

  system_status = status_gt_tx( gt_status );

  return system_status;
}

int XMMRegs_RocketIO_RxSystem_Status(XMMRegs *InstancePtr, int transceiver)
{
  int system_status;
  XMMRegs_gt_status *gt_status;

  gt_status = (XMMRegs_gt_status *)(InstancePtr->BaseAddress + XMMR_GT_STATUS_OFFSET);

  system_status = status_gt_rx( gt_status );

  return system_status;
}

int XMMRegs_RocketIO_MgtData_Set(XMMRegs *InstancePtr, int transceiver, unsigned int msb, unsigned int lsb, unsigned char comma_msb, unsigned char comma_lsb)
{
  XMMRegs_txmgtdata_ctrl *reg;
  void *ba = InstancePtr->BaseAddress;

  reg = (XMMRegs_txmgtdata_ctrl *)(ba +  XMMR_MGT_REGDATA_CTRL_OFFSET);

  if ( ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) ||
       ( (msb > 0xFF) || (lsb > 0xFF) ) ||
       ( (comma_msb != COMMA_ON) && (comma_msb != COMMA_OFF) ) ||
       ( (comma_lsb != COMMA_ON) && (comma_lsb != COMMA_OFF) ) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_MgtData_Set\n");
    return XST_FAILURE;
  }

  set_regdata(reg, msb, lsb, comma_msb, comma_lsb);

  return XST_SUCCESS; 
}

void XMMRegs_RocketIO_TriggerRstCarrier_Set(XMMRegs *InstancePtr)
{
  XMMRegs_gt_ctrl *gt;

  gt = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  gt->trigger_rst_carrier = 1; 
}

void XMMRegs_RocketIO_TriggerRstCarrier_Unset(XMMRegs *InstancePtr)
{
  XMMRegs_gt_ctrl *gt;

  gt = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  gt->trigger_rst_carrier = 0; 
}

/************************************************************

	      INIT, RESET and PRINT FUNCTIONS

************************************************************/

void XMMRegs_RocketIO_Setup(XMMRegs *InstancePtr)
{
  XMMRegs_gt_ctrl *gt;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;
  XMMRegs_txmgtdata_ctrl *reg;
  void *ba;

  ba = InstancePtr->BaseAddress;
  gt = (XMMRegs_gt_ctrl *)(ba +  XMMR_GT_CTRL_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba +  XMMR_GT_DRP_ADDR_CTRL_OFFSET); 
  drp = (XMMRegs_drp_ctrl *)(ba +  XMMR_GT_DRP_CTRL_OFFSET);
  reg = (XMMRegs_txmgtdata_ctrl *)(ba +  XMMR_MGT_REGDATA_CTRL_OFFSET);

  set_regdata(reg, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);

  init_drp_addr(drp_addr);

  init_drp(drp);

  XMMRegs_RocketIO_RefClk_Set(InstancePtr);

  setup_gt(gt);
}

void XMMRegs_RocketIO_Stop(XMMRegs *InstancePtr)
{
  XMMRegs_gt_ctrl *gt;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;
  XMMRegs_txmgtdata_ctrl *reg;
  void *ba;

  ba = InstancePtr->BaseAddress;
  gt = (XMMRegs_gt_ctrl *)(ba +  XMMR_GT_CTRL_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba +  XMMR_GT_DRP_ADDR_CTRL_OFFSET); 
  drp = (XMMRegs_drp_ctrl *)(ba +  XMMR_GT_DRP_CTRL_OFFSET);
  reg = (XMMRegs_txmgtdata_ctrl *) (ba +  XMMR_MGT_REGDATA_CTRL_OFFSET);

  set_regdata(reg, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);

  init_drp_addr(drp_addr);

  stop_drp(drp);

  stop_gt(gt);

  XMMRegs_RocketIO_RefClk_Set(InstancePtr);
}

void XMMRegs_RocketIO_Start(XMMRegs *InstancePtr)
{
  XMMRegs_gt_ctrl *gt;
  void *ba;

  ba = InstancePtr->BaseAddress;

  gt = (XMMRegs_gt_ctrl *)(ba +  XMMR_GT_CTRL_OFFSET);

  init_gt(gt);
}

void XMMRegs_RocketIO_Init(XMMRegs *InstancePtr)
{
  XMMRegs_RocketIO_Setup(InstancePtr);
  XMMRegs_RocketIO_Start(InstancePtr);
}

void XMMRegs_RocketIO_Reset(XMMRegs *InstancePtr)
{
  XMMRegs_RocketIO_Stop(InstancePtr);
  XMMRegs_RocketIO_Init(InstancePtr);
}

void XMMRegs_RocketIO_PrintAll(XMMRegs *InstancePtr)
{
  DBG(DBLI, "\nPrintAll of RocketIO :---------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "gt_ctrl\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_GT_CTRL_OFFSET);
  DBG(DBLI, "gt_status\t\t:\t"); 
  XMMRegs_PrintBinary(InstancePtr, XMMR_GT_STATUS_OFFSET);
  DBG(DBLI, "gt_rxdata\t\t:\t");
  XMMRegs_PrintHex(InstancePtr, XMMR_GT_RXDATA_OFFSET);
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "drp_addr_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  DBG(DBLI, "drp_ctrl\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_GT_DRP_CTRL_OFFSET);
  DBG(DBLI, "drp_status\t\t:\t");
  XMMRegs_PrintBinary(InstancePtr, XMMR_GT_DRP_STATUS_OFFSET);
  DBG(DBLI, "gt_txdata_status\t:\t");
  XMMRegs_PrintHex(InstancePtr, XMMR_GT_TXDATA_STATUS_OFFSET );
}

#undef DBG
