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

static unsigned int set_drp_choice(int transceiver)
{
  switch (transceiver) 
  {
  case NO_TRANSCEIVER :
  	return 0;
  case TRANSCEIVER_0 : 
	return 1;
  case TRANSCEIVER_1 : case TRANSCEIVER_2 : case TRANSCEIVER_3 :
	return (1<<transceiver);
  default :
        DBG(DBLE, "ERROR : in function set_drp_choice\n");
        DBG(DBLE, "ERROR : transceiver out of range\n");
        DBG(DBLE, "ERROR : transceiver = %d\n", transceiver);
	return 0;
  }  
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

  return;
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

static unsigned short change_bit(unsigned short word_value, unsigned char bit_pos, unsigned char bit_value)
{
  unsigned short new_word_value;

  if ( (bit_pos > 15) || (bit_value > 1) ) {
    DBG(DBLE, "ERROR : out of range error in function change_bit\n");
    return word_value;  
  }

  if (bit_value)
    new_word_value = ( (1 << bit_pos) | word_value );
  else
    new_word_value = ( ( ~(1 << bit_pos) ) & word_value );

  return new_word_value;
}

static unsigned short read_word_drp(XMMRegs_drp_addr_ctrl *drp_addr, XMMRegs_drp_ctrl *drp, XMMRegs_drp_status *drp_status, unsigned char DADDR)
{
  unsigned short val;

  drp_addr->drp_addr = DADDR;
  drp->drp_en = 0;
  drp->drp_we = 0;
  drp->drp_en = 1;
  drp->drp_en = 0;

  while (drp_status->drp_bsy == 1) ;

  val = (unsigned short)(drp_status->drp_do);

  DBG(DBLD, "read value at drp address : 0x%X : ", DADDR);
  print_u16_binary(&val);

  return val;
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

int XMMRegs_RocketIO_RefClkRX_Diagnose (XMMRegs *InstancePtr, int transceiver)
{
  return 1; // NUMEXO2
}

/* looks if there is a comma alignment */
int XMMRegs_RocketIO_CommaAlign_Diagnose (XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_status *s;
  void *ba;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error for transceiver in function XMMRegs_RocketIO_CommaAlign_Diagnose\n");
    return -1;
  }

  ba = InstancePtr->BaseAddress;
  s = (XMMRegs_gt_status *)(ba + XMMR_GT_STATUS_OFFSET + 0x4*transceiver);

  if ( XMMRegs_RocketIO_RxSystem_Status(InstancePtr, transceiver) == READY )
  {
// F. Saillant le 30 novembre 2012 : on autorise aussi COMMA (0x00BC) car c'est ce que les mezzanines GTS v3 envoient ...

//    if ((s->rxcommadet == 1) && (s->rxchariscomma == 1) && ((XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, transceiver) == ALIGN_WORD) || (XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, transceiver) == COMMA)))
    if ((XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, transceiver) == ALIGN_WORD) || (XMMRegs_RocketIO_RxMgtdata_Read(InstancePtr, transceiver) == COMMA))
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

int XMMRegs_RocketIO_RefClkTX_Diagnose (XMMRegs *InstancePtr, int tile)
{
  return 1; // NUMEXO2
}

int XMMRegs_RocketIO_RefClkTX_Set(XMMRegs *InstancePtr, int tile, unsigned char refclk)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  void *ba;

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  set_word_drp(drp_addr, drp, drp_status, 0, 0x04, 0xA0E9); // for GTS LEAF NUMEXO2 (Virtex5)

  return XST_SUCCESS;
}

int XMMRegs_RocketIO_RefClkRX_Set(XMMRegs *InstancePtr, int transceiver, unsigned char refclk)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  void *ba;

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  set_word_drp(drp_addr, drp, drp_status, 0, 0x04, 0xA0E9); // for GTS LEAF NUMEXO2 (Virtex5)

  return XST_SUCCESS;
}

/************************************************************

	Lowlevel INIT and RESET FUNCTIONS

************************************************************/

int XMMRegs_RocketIO_TxSystem_Stop(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS; 
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;  

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress +  XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_TxSystem_Stop\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    reset_gt_tx( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_RxSystem_Stop(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS; 
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;  

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress +  XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_RxSystem_Stop\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    reset_gt_rx( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_TxSystem_Init(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_TxSystem_Init\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    init_gt_tx( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_RxSystem_Init(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_RxSystem_Init\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    init_gt_rx( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_TxSystem_Reset(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_RocketIO_TxSystem_Stop(InstancePtr, transceiver);
  status |= XMMRegs_RocketIO_TxSystem_Init(InstancePtr, transceiver);

  return status;
}

int XMMRegs_RocketIO_RxSystem_Reset(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_RocketIO_RxSystem_Stop(InstancePtr, transceiver);
  status |= XMMRegs_RocketIO_RxSystem_Init(InstancePtr, transceiver);

  return status;
}

int XMMRegs_RocketIO_RxPcs_Init(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_RxPcs_Init\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    init_gt_rx_pcs( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_TxPcs_Init(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_TxPcs_Init\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    init_gt_tx_pcs( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_RxPcs_Reset(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_RxPcs_Reset\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    reset_gt_rx_pcs( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_TxPcs_Reset(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  unsigned int i;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_TxPcs_Reset\n");
    status = XST_FAILURE;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    reset_gt_tx_pcs( gt0 + i );
  }

  return status;
}

int XMMRegs_RocketIO_TxSystem_Status(XMMRegs *InstancePtr, int transceiver)
{
  int system_status;
  XMMRegs_gt_status *gt_status;
  unsigned int i;

  gt_status = (XMMRegs_gt_status *)(InstancePtr->BaseAddress + XMMR_GT_STATUS_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_TxSystem_Status\n");
    system_status = C_NULL;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    system_status = status_gt_tx( gt_status + i );
  }

  return system_status;
}

int XMMRegs_RocketIO_RxSystem_Status(XMMRegs *InstancePtr, int transceiver)
{
  int system_status;
  XMMRegs_gt_status *gt_status;
  unsigned int i;

  gt_status = (XMMRegs_gt_status *)(InstancePtr->BaseAddress + XMMR_GT_STATUS_OFFSET);

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_RxSystem_Status\n");
    system_status = C_NULL;
  }
  else {
    i = transceiver - TRANSCEIVER_0;
    system_status = status_gt_rx( gt_status + i );
  }

  return system_status;
}

int XMMRegs_RocketIO_MgtData_Set(XMMRegs *InstancePtr, int transceiver, unsigned int msb, unsigned int lsb, unsigned char comma_msb, unsigned char comma_lsb)
{
  XMMRegs_txmgtdata_ctrl *reg0, *reg;
  void *ba = InstancePtr->BaseAddress;

  reg0 = (XMMRegs_txmgtdata_ctrl *) (ba +  XMMR_MGT_REGDATA_CTRL_OFFSET);

  if ( ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) ||
       ( (msb > 0xFF) || (lsb > 0xFF) ) ||
       ( (comma_msb != COMMA_ON) && (comma_msb != COMMA_OFF) ) ||
       ( (comma_lsb != COMMA_ON) && (comma_lsb != COMMA_OFF) ) ) {
    DBG(DBLE, "ERROR : out of range in function XMMRegs_RocketIO_MgtData_Set\n");
    return XST_FAILURE;
  }

  reg = reg0 + transceiver;
  set_regdata(reg, msb, lsb, comma_msb, comma_lsb);

  return XST_SUCCESS; 
}


/************************************************************

	      INIT, RESET and PRINT FUNCTIONS

************************************************************/

int XMMRegs_RocketIO_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;
  XMMRegs_txmgtdata_ctrl *reg0;
  void *ba;

  ba = InstancePtr->BaseAddress;
  gt0 = (XMMRegs_gt_ctrl *)(ba +  XMMR_GT_CTRL_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba +  XMMR_GT_DRP_ADDR_CTRL_OFFSET); 
  drp = (XMMRegs_drp_ctrl *)(ba +  XMMR_GT_DRP_CTRL_OFFSET);
  reg0 = (XMMRegs_txmgtdata_ctrl *) (ba +  XMMR_MGT_REGDATA_CTRL_OFFSET);

  set_regdata(reg0, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);

  init_drp_addr(drp_addr);

  init_drp(drp);

  status |= XMMRegs_RocketIO_RefClkTX_Set(InstancePtr, TILE_0, REFCLK2);

//  status |= XMMRegs_RocketIO_RefClkRX_Set(InstancePtr, TRANSCEIVER_0, REFCLK2); // inutile car redondant avec XMMRegs_RocketIO_RefClkTX_Set

  if ( XMMRegs_IsTransceiverConnected(InstancePtr, TRANSCEIVER_0) == TRANSCEIVER_IS_CONNECTED)
  {
    if ( XMMRegs_IsTransceiverInGtsTree(InstancePtr, TRANSCEIVER_0) == TRANSCEIVER_IS_IN_GTS_TREE )
    {
      setup_gt(gt0);

//      XMMRegs_RocketIO_SerialLoopback_Set(InstancePtr, TRANSCEIVER_0, SERIAL_LOOPBACK_OFF);
    }
    else if ( XMMRegs_IsTransceiverConnectedToDigitizer(InstancePtr, TRANSCEIVER_0) == TRANSCEIVER_IS_CONNECTED_TO_DIGITIZER)
    { /* the RocketIO is bypassed */
      /* puts the transceiver in the unused mode */
//      XMMRegs_RocketIO_UnusedMode_Set(InstancePtr, TRANSCEIVER_0);
    }
    else
    { /* it is connected neither to the GTS tree nor to the digitizer */
//      XMMRegs_RocketIO_UnusedMode_Set(InstancePtr, TRANSCEIVER_0);
    }
  }
  else
  { /* the transceiver is not connected */
    /* puts the transceiver in the unused mode */
//    XMMRegs_RocketIO_UnusedMode_Set(InstancePtr, TRANSCEIVER_0);
  }

  return status;
}

int XMMRegs_RocketIO_TriggerRstCarrier_Set(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt0;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  gt0->trigger_rst_carrier = 1; 

  return XST_SUCCESS;
}

int XMMRegs_RocketIO_TriggerRstCarrier_Unset(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt0;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  gt0->trigger_rst_carrier = 0; 

  return XST_SUCCESS;
}
/*
int XMMRegs_RocketIO_TxDataSel_Set(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt0;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

//  gt0->choice_refclk = 1; 

  return XST_SUCCESS;
}
*/
/*
int XMMRegs_RocketIO_TxDataSel_Unset(XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_gt_ctrl *gt0;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

//  gt0->choice_refclk = 0; 

  return XST_SUCCESS;
}
*/
/* the RocketIO is set in the unused mode by software
This code has to be improved.
Ideally, it should shut down the transceiver */
/*
int XMMRegs_RocketIO_UnusedMode_Set(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;

  gt0 = (XMMRegs_gt_ctrl *)(InstancePtr->BaseAddress + XMMR_GT_CTRL_OFFSET);

  stop_gt(gt0 + transceiver); 

  status |= XMMRegs_RocketIO_SerialLoopback_Set(InstancePtr, transceiver, SERIAL_LOOPBACK_ON);

  return XST_SUCCESS;
}
*/
int XMMRegs_RocketIO_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;
  XMMRegs_txmgtdata_ctrl *reg0;
  void *ba;

  ba = InstancePtr->BaseAddress;
  gt0 = (XMMRegs_gt_ctrl *)(ba +  XMMR_GT_CTRL_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba +  XMMR_GT_DRP_ADDR_CTRL_OFFSET); 
  drp = (XMMRegs_drp_ctrl *)(ba +  XMMR_GT_DRP_CTRL_OFFSET);
  reg0 = (XMMRegs_txmgtdata_ctrl *) (ba +  XMMR_MGT_REGDATA_CTRL_OFFSET);

  set_regdata(reg0, TEST_POLARITY, COMMA, COMMA_OFF, COMMA_ON);

  init_drp_addr(drp_addr);

  stop_drp(drp);

  stop_gt(gt0);

  status |= XMMRegs_RocketIO_RefClkTX_Set(InstancePtr, TILE_0, REFCLK2);

//  status |= XMMRegs_RocketIO_RefClkRX_Set(InstancePtr, TRANSCEIVER_0, REFCLK2); // inutile car redondant avec XMMRegs_RocketIO_RefClkTX_Set

//  status |= XMMRegs_RocketIO_SerialLoopback_Set(InstancePtr, TRANSCEIVER_0, SERIAL_LOOPBACK_OFF);

  return status;
}

int XMMRegs_RocketIO_Start(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  XMMRegs_gt_ctrl *gt0;
  void *ba;

  ba = InstancePtr->BaseAddress;

  gt0 = (XMMRegs_gt_ctrl *)(ba +  XMMR_GT_CTRL_OFFSET);

  init_gt_tx(gt0);

  init_gt_rx(gt0);

  return status;
}

int XMMRegs_RocketIO_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

//  DBG(DBLD, "RocketIO_Init --->\n");
  status |= XMMRegs_RocketIO_Setup(InstancePtr);
  status |= XMMRegs_RocketIO_Start(InstancePtr);
//  DBG(DBLD, "---> RocketIO_Init |\n");

  return status;
}

int XMMRegs_RocketIO_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status |= XMMRegs_RocketIO_Stop(InstancePtr);
  status |= XMMRegs_RocketIO_Init(InstancePtr);

  return status;
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

  return;
}

#undef DBG
