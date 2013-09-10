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

static int read_u16_bit(unsigned short val, unsigned char bit_pos)
{ 
  unsigned short tmp;

  tmp = val << (15 - bit_pos);
  tmp = tmp >> 15;
  return (int) ( tmp );
}

/* is the address of pointer p between baseaddres ba and highaddress ba+offset ? */
/*
static unsigned char debug_address(unsigned int p, char *str, unsigned int ba, unsigned int offset)
{
  if ( (p < ba) || (p > ba+offset ) )
  {
  DBG(DBLE, "ERROR : in function %s\n", str);
  DBG(DBLE, "ERROR : 0x%08X out of range", p);
  return 0;
  }

  DBG(DBLD, "address : 0x%08X\n", p); 

  return 1;
}
*/

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

  return;
}

static void setup_gt(XMMRegs_gt_ctrl *gt)
{
//  gt->txchardispval = 0;
//  gt->txchardispmode = 0;
//  gt->rx_pcs_reset = 0;
//  gt->tx_pcs_reset = 0;
  gt->gtsStreamReady = 1;
  gt->rx_system_reset = 1;
  gt->tx_system_reset = 1;
  gt->backpressure = 0;
  gt->trigger_rst_carrier = 1;

  return;
}

static void stop_gt(XMMRegs_gt_ctrl *gt)
{
//  gt->txchardispval = 0;
//  gt->txchardispmode = 0;
//  gt->rx_pcs_reset = 0;
//  gt->tx_pcs_reset = 0;
  gt->gtsStreamReady = 1;
  gt->rx_system_reset = 1;
  gt->tx_system_reset = 1;
  gt->backpressure = 0;
  gt->trigger_rst_carrier = 1;

  return;
}

static void init_gt_tx(XMMRegs_gt_ctrl *gt)
{
  gt->tx_system_reset = 0;

  return;
}

static void init_gt_rx(XMMRegs_gt_ctrl *gt)
{
  gt->rx_system_reset = 0;

  return;
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

static void reset_gt_tx(XMMRegs_gt_ctrl *gt)
{
  gt->tx_system_reset = 1;

  return;
}

static void reset_gt_rx(XMMRegs_gt_ctrl *gt)
{
  gt->rx_system_reset = 1;

  return;
}

static void reset_gt_tx_pcs(XMMRegs_gt_ctrl *gt)
{
  gt->tx_pcs_reset = 1;

  return;
}

static void reset_gt_rx_pcs(XMMRegs_gt_ctrl *gt)
{
  gt->rx_pcs_reset = 1;

  return;
}

static void init_gt_tx_pcs(XMMRegs_gt_ctrl *gt)
{
  gt->tx_pcs_reset = 0;

  return;
}

static void init_gt_rx_pcs(XMMRegs_gt_ctrl *gt)
{
  gt->rx_pcs_reset = 0;

  return;
}

static void init_drp_addr(XMMRegs_drp_addr_ctrl *drp_addr)
{
  drp_addr->drp_choice = set_drp_choice(NO_TRANSCEIVER);
  drp_addr->drp_addr = 0;

  return;
}

static void init_drp(XMMRegs_drp_ctrl *drp)
{
  drp->signal_detect = 0xF;
  drp->RST_drp_bsy = 0;
  drp->drp_we = 0;
  drp->drp_en = 0;
  drp->drp_di = 0;

  return;
}

static void stop_drp(XMMRegs_drp_ctrl *drp)
{
  drp->signal_detect = 0x0;
  drp->RST_drp_bsy = 0;
  drp->drp_we = 0;
  drp->drp_en = 0;
  drp->drp_di = 0;

  return;
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

static unsigned short read_word_drp(XMMRegs_drp_addr_ctrl *drp_addr, XMMRegs_drp_ctrl *drp, XMMRegs_drp_status *drp_status, unsigned char transceiver, unsigned char DADDR)
{
  unsigned short val;

  drp_addr->drp_choice = set_drp_choice((int)transceiver);
  drp_addr->drp_addr = DADDR;
  drp->drp_en = 0;
  drp->drp_we = 0;
  drp->drp_en = 1;
  drp->drp_en = 0;
  while (drp_status->drp_bsy == 1) ;

  val = (unsigned short) (drp_status->drp_do);

  DBG(DBLD, "read value at drp address : 0x%X : ", DADDR);
  print_u16_binary(&val);

  return val;
}

static void set_word_drp(XMMRegs_drp_addr_ctrl *drp_addr, XMMRegs_drp_ctrl *drp, XMMRegs_drp_status *drp_status, unsigned char transceiver, unsigned char DADDR, unsigned short DI)
{
  drp_addr->drp_choice = set_drp_choice((int)transceiver);
  drp_addr->drp_addr = DADDR;
  drp->drp_di = DI;
  drp->drp_en = 0;
  drp->drp_we = 1;
  drp->drp_en = 1;
  drp->drp_en = 0;
  drp->drp_we = 1;

  while (drp_status->drp_bsy == 1) ;

  return;
}

static int read_bit_drp(XMMRegs_drp_addr_ctrl *drp_addr, XMMRegs_drp_ctrl *drp, XMMRegs_drp_status *drp_status, unsigned char transceiver, unsigned char DADDR, unsigned char bit_pos)
{
  unsigned short word_value;

  word_value = read_word_drp(drp_addr, drp, drp_status, transceiver, DADDR);

  return read_u16_bit(word_value, bit_pos);
}

static void set_bit_drp(XMMRegs_drp_addr_ctrl *drp_addr, XMMRegs_drp_ctrl *drp, XMMRegs_drp_status *drp_status, unsigned char transceiver, unsigned char DADDR, unsigned char bit_pos, unsigned char bit_value)
{
  unsigned short word_value, new_word_value;

  word_value = read_word_drp(drp_addr, drp, drp_status, transceiver, DADDR);
//  DBG(DBLD, "Old bit value was : %d\n", read_u16_bit(word_value, bit_pos) );
  new_word_value = change_bit(word_value, bit_pos, bit_value);
//  DBG(DBLD, "Word value to be sent : "); print_u16_binary(&new_word_value); 
  set_word_drp(drp_addr, drp, drp_status, transceiver, DADDR, new_word_value);
//  DBG(DBLD, "After DRP programming :\n");
  word_value = read_word_drp(drp_addr, drp, drp_status, transceiver, DADDR); /* to control */
//  DBG(DBLD, "New bit value is : %d\n", read_u16_bit(word_value, bit_pos) );    

  return;
}

/************************************************************

		HIGH LEVEL FUNCTIONS

************************************************************/

int XMMRegs_RocketIO_DrpWord_Read(XMMRegs *InstancePtr, int transceiver, unsigned char DADDR)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;
  void *ba;

  if ( (transceiver < NO_TRANSCEIVER) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_read_word_drp\n");
    DBG(DBLE, "ERROR : value of transceiver should be between %d and %d\n", NO_TRANSCEIVER, TRANSCEIVER_3);

    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  if (transceiver > NO_TRANSCEIVER) {
    read_word_drp(drp_addr, drp, drp_status, (unsigned char)transceiver, DADDR);
  }

  return XST_SUCCESS;
}
/*
int XMMRegs_RocketIO_SerialLoopback_Set(XMMRegs *InstancePtr, int transceiver, unsigned char serial_loopback)
{
  XMMRegs_gt_ctrl *gt;
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  unsigned char TXPOST_TAP_PD;
  void *ba;

  if ( (transceiver < NO_TRANSCEIVER) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_handle_serial_loopback\n");
    DBG(DBLE, "ERROR : value of transceiver should be between %d and %d\n", NO_TRANSCEIVER, TRANSCEIVER_3);

    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  if (transceiver > NO_TRANSCEIVER) {
    gt = (XMMRegs_gt_ctrl *)(ba +  XMMR_GT_CTRL_OFFSET + 0x4*transceiver);
    if (serial_loopback == SERIAL_LOOPBACK_ON) {
      TXPOST_TAP_PD = 0;
      gt->loopback = 3;
    }
    else {
      TXPOST_TAP_PD = 1;
      gt->loopback = 0;
    }

//    switch (transceiver) 
//    {
//      case TRANSCEIVER_0 : case TRANSCEIVER_2 :
//        set_bit_drp(drp_addr, drp, drp_status, (unsigned char)transceiver, 0x4E, 12, TXPOST_TAP_PD);
//        break;
//      case TRANSCEIVER_1 : case TRANSCEIVER_3 :
//        set_bit_drp(drp_addr, drp, drp_status, (unsigned char)transceiver, 0x4C, 12, TXPOST_TAP_PD);
//        break;
//
//      case TRANSCEIVER_0 :
//        set_word_drp(drp_addr, drp, drp_status, (unsigned char)transceiver, 0x04, 0xA0D9);
//        break;
//
//      default :
//        DBG(DBLI, "STRANGE : no transceiver selected\n");
//        break;
//    }  
  }
  else {
    DBG(DBLI, "STRANGE : no transceiver selected for serial loopback setting\\n");
  }

  return XST_SUCCESS;
}
*/
/* returns the reference clock used by the RX transceiver */
int XMMRegs_RocketIO_RefClkRX_Diagnose (XMMRegs *InstancePtr, int transceiver)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  int RXPMACLKSEL_0 = NO_REFCLK;
  void *ba;

  if ( (transceiver < NO_TRANSCEIVER) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_RocketIO_RefClkRX_Diagnose \n");
    DBG(DBLE, "ERROR : value of transceiver should be between %d and %d\n", NO_TRANSCEIVER, TRANSCEIVER_3);
    return NO_REFCLK; // NO_REFCLK = -1
  }

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  switch (transceiver) 
  {
    case TRANSCEIVER_0 : // for GTS LEAF NUMEXO2 (Virtex5)
      RXPMACLKSEL_0 = 1;
      break;

    default :
      DBG(DBLI, "STRANGE : no transceiver selected\n");
      break;
  }  

  return RXPMACLKSEL_0;
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

/* returns the reference clock used by the TX transceiver */
int XMMRegs_RocketIO_RefClkTX_Diagnose (XMMRegs *InstancePtr, int tile)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  void *ba;

  if ( (tile != TILE_0) && (tile != TILE_1) ) {
    DBG(DBLE, "ERROR : out of range error for tile in function XMMRegs_RocketIO_RefClkTX_Diagnose\n");
    return NO_REFCLK; // NO_RECLK = -1
  }

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  return REFCLK2; // for GTS LEAF NUMEXO2 (Virtex5) // REFCLK2 = 1
}

int XMMRegs_RocketIO_RefClkTX_Set(XMMRegs *InstancePtr, int tile, unsigned char refclk)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  unsigned char TXABPMACLKSEL_0;
  void *ba;

  if ( (tile != TILE_0) && (tile != TILE_1) ) {
    DBG(DBLE, "ERROR : out of range error for tile in function XMMRegs_RocketIO_RefClkTX_Set\n");
    return XST_FAILURE;
  }

  if ( (refclk != REFCLK1) && (refclk != REFCLK2) ) {
    DBG(DBLE, "ERROR : out of range error for refclk in function XMMRegs_RocketIO_RefClkTX_Set\n");
    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  if (refclk == REFCLK2) {
    TXABPMACLKSEL_0 = REFCLK2;
  }
  else {
    TXABPMACLKSEL_0 = REFCLK1;
  }

  set_word_drp(drp_addr, drp, drp_status, 0, 0x04, 0xA0E9); // for GTS LEAF NUMEXO2 (Virtex5)

  return XST_SUCCESS;
}

int XMMRegs_RocketIO_RefClkRX_Set(XMMRegs *InstancePtr, int transceiver, unsigned char refclk)
{
  XMMRegs_drp_status *drp_status;
  XMMRegs_drp_addr_ctrl *drp_addr;
  XMMRegs_drp_ctrl *drp;  
  unsigned char RXPMACLKSEL_0;
  void *ba;

  if ( (transceiver < NO_TRANSCEIVER) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_RocketIO_RefClkRX_Set\n");
    DBG(DBLE, "ERROR : value of transceiver should be between %d and %d\n", NO_TRANSCEIVER, TRANSCEIVER_3);
    return XST_FAILURE;
  }

  if ( (refclk != REFCLK1) && (refclk != REFCLK2) ) {
    DBG(DBLE, "ERROR : out of range error for refclk in function XMMRegs_RocketIO_RefClkRX_Set\n");
    return XST_FAILURE;
  }

  ba = InstancePtr->BaseAddress;
  drp_status = (XMMRegs_drp_status *)(ba + XMMR_GT_DRP_STATUS_OFFSET);
  drp_addr = (XMMRegs_drp_addr_ctrl *)(ba + XMMR_GT_DRP_ADDR_CTRL_OFFSET);
  drp = (XMMRegs_drp_ctrl *)(ba + XMMR_GT_DRP_CTRL_OFFSET);

  if (refclk == REFCLK2) {
    RXPMACLKSEL_0 = REFCLK2;
  }
  else {
    RXPMACLKSEL_0 = REFCLK1;
  }

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
