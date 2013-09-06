/* $Id: xstatus.h,v 1.28 2007/10/22 08:25:41 svemula Exp $ */
/******************************************************************************
*
*       XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
*       AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
*       SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
*       OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
*       APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
*       THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
*       AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
*       FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
*       WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
*       IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
*       REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
*       INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*       FOR A PARTICULAR PURPOSE.
*
*       (c) Copyright 2002-2007 Xilinx Inc.
*       All rights reserved.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xstatus.h
*
* This file contains Xilinx software status codes.  Status codes have their
* own data type called int.  These codes are used throughout the Xilinx
* device drivers.
*
******************************************************************************/

#ifndef XSTATUS_H		/* prevent circular inclusions */
#define XSTATUS_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

//#include "xbasic_types.h"

/************************** Constant Definitions *****************************/

/*********************** Common statuses 0 - 500 *****************************/

#define XST_SUCCESS                     0
#define XST_FAILURE                     1
#define XST_DEVICE_NOT_FOUND            2
#define XST_DEVICE_BLOCK_NOT_FOUND      3
#define XST_INVALID_VERSION             4
#define XST_DEVICE_IS_STARTED           5
#define XST_DEVICE_IS_STOPPED           6
#define XST_FIFO_ERROR                  7	/* an error occurred during an
						   operation with a FIFO such as
						   an underrun or overrun, this
						   error requires the device to
						   be reset */
#define XST_RESET_ERROR                 8	/* an error occurred which requires
						   the device to be reset */
#define XST_DMA_ERROR                   9	/* a DMA error occurred, this error
						   typically requires the device
						   using the DMA to be reset */
#define XST_NOT_POLLED                  10	/* the device is not configured for
						   polled mode operation */
#define XST_FIFO_NO_ROOM                11	/* a FIFO did not have room to put
						   the specified data into */
#define XST_BUFFER_TOO_SMALL            12	/* the buffer is not large enough
						   to hold the expected data */
#define XST_NO_DATA                     13	/* there was no data available */
#define XST_REGISTER_ERROR              14	/* a register did not contain the
						   expected value */
#define XST_INVALID_PARAM               15	/* an invalid parameter was passed
						   into the function */
#define XST_NOT_SGDMA                   16	/* the device is not configured for
						   scatter-gather DMA operation */
#define XST_LOOPBACK_ERROR              17	/* a loopback test failed */
#define XST_NO_CALLBACK                 18	/* a callback has not yet been
						   registered */
#define XST_NO_FEATURE                  19	/* device is not configured with
						   the requested feature */
#define XST_NOT_INTERRUPT               20	/* device is not configured for
						   interrupt mode operation */
#define XST_DEVICE_BUSY                 21	/* device is busy */
#define XST_ERROR_COUNT_MAX             22	/* the error counters of a device
						   have maxed out */
#define XST_IS_STARTED                  23	/* used when part of device is
						   already started i.e.
						   sub channel */
#define XST_IS_STOPPED                  24	/* used when part of device is
						   already stopped i.e.
						   sub channel */
#define XST_DATA_LOST                   26	/* driver defined error */
#define XST_RECV_ERROR                  27	/* generic receive error */
#define XST_SEND_ERROR                  28	/* generic transmit error */
#define XST_NOT_ENABLED                 29	/* a requested service is not
						   available because it has not
						   been enabled */

/***************** Utility Component statuses 401 - 500  *********************/

#define XST_MEMTEST_FAILED              401	/* memory test failed */


/***************** Common Components statuses 501 - 1000 *********************/

/********************* Packet Fifo statuses 501 - 510 ************************/

#define XST_PFIFO_LACK_OF_DATA          501	/* not enough data in FIFO   */
#define XST_PFIFO_NO_ROOM               502	/* not enough room in FIFO   */
#define XST_PFIFO_BAD_REG_VALUE         503	/* self test, a register value
						   was invalid after reset */
#define XST_PFIFO_ERROR                 504	/* generic packet FIFO error */
#define XST_PFIFO_DEADLOCK              505	/* packet FIFO is reporting
						 * empty and full simultaneously
						 */

/************************** DMA statuses 511 - 530 ***************************/

#define XST_DMA_TRANSFER_ERROR          511	/* self test, DMA transfer
						   failed */
#define XST_DMA_RESET_REGISTER_ERROR    512	/* self test, a register value
						   was invalid after reset */
#define XST_DMA_SG_LIST_EMPTY           513	/* scatter gather list contains
						   no buffer descriptors ready
						   to be processed */
#define XST_DMA_SG_IS_STARTED           514	/* scatter gather not stopped */
#define XST_DMA_SG_IS_STOPPED           515	/* scatter gather not running */
#define XST_DMA_SG_LIST_FULL            517	/* all the buffer desciptors of
						   the scatter gather list are
						   being used */
#define XST_DMA_SG_BD_LOCKED            518	/* the scatter gather buffer
						   descriptor which is to be
						   copied over in the scatter
						   list is locked */
#define XST_DMA_SG_NOTHING_TO_COMMIT    519	/* no buffer descriptors have been
						   put into the scatter gather
						   list to be commited */
#define XST_DMA_SG_COUNT_EXCEEDED       521	/* the packet count threshold
						   specified was larger than the
						   total # of buffer descriptors
						   in the scatter gather list */
#define XST_DMA_SG_LIST_EXISTS          522	/* the scatter gather list has
						   already been created */
#define XST_DMA_SG_NO_LIST              523	/* no scatter gather list has
						   been created */
#define XST_DMA_SG_BD_NOT_COMMITTED     524	/* the buffer descriptor which was
						   being started was not committed
						   to the list */
#define XST_DMA_SG_NO_DATA              525	/* the buffer descriptor to start
						   has already been used by the
						   hardware so it can't be reused
						 */
#define XST_DMA_SG_LIST_ERROR           526	/* general purpose list access
						   error */
#define XST_DMA_BD_ERROR                527	/* general buffer descriptor
						   error */

/************************** IPIF statuses 531 - 550 ***************************/

#define XST_IPIF_REG_WIDTH_ERROR        531	/* an invalid register width
						   was passed into the function */
#define XST_IPIF_RESET_REGISTER_ERROR   532	/* the value of a register at
						   reset was not valid */
#define XST_IPIF_DEVICE_STATUS_ERROR    533	/* a write to the device interrupt
						   status register did not read
						   back correctly */
#define XST_IPIF_DEVICE_ACK_ERROR       534	/* the device interrupt status
						   register did not reset when
						   acked */
#define XST_IPIF_DEVICE_ENABLE_ERROR    535	/* the device interrupt enable
						   register was not updated when
						   other registers changed */
#define XST_IPIF_IP_STATUS_ERROR        536	/* a write to the IP interrupt
						   status register did not read
						   back correctly */
#define XST_IPIF_IP_ACK_ERROR           537	/* the IP interrupt status register
						   did not reset when acked */
#define XST_IPIF_IP_ENABLE_ERROR        538	/* IP interrupt enable register was
						   not updated correctly when other
						   registers changed */
#define XST_IPIF_DEVICE_PENDING_ERROR   539	/* The device interrupt pending
						   register did not indicate the
						   expected value */
#define XST_IPIF_DEVICE_ID_ERROR        540	/* The device interrupt ID register
						   did not indicate the expected
						   value */
#define XST_IPIF_ERROR                  541	/* generic ipif error */

/****************** Device specific statuses 1001 - 4095 *********************/

/********************* Ethernet statuses 1001 - 1050 *************************/

#define XST_EMAC_MEMORY_SIZE_ERROR  1001	/* Memory space is not big enough
						 * to hold the minimum number of
						 * buffers or descriptors */
#define XST_EMAC_MEMORY_ALLOC_ERROR 1002	/* Memory allocation failed */
#define XST_EMAC_MII_READ_ERROR     1003	/* MII read error */
#define XST_EMAC_MII_BUSY           1004	/* An MII operation is in progress */
#define XST_EMAC_OUT_OF_BUFFERS     1005	/* Driver is out of buffers */
#define XST_EMAC_PARSE_ERROR        1006	/* Invalid driver init string */
#define XST_EMAC_COLLISION_ERROR    1007	/* Excess deferral or late
						 * collision on polled send */

/*********************** UART statuses 1051 - 1075 ***************************/
#define XST_UART

#define XST_UART_INIT_ERROR         1051
#define XST_UART_START_ERROR        1052
#define XST_UART_CONFIG_ERROR       1053
#define XST_UART_TEST_FAIL          1054
#define XST_UART_BAUD_ERROR         1055
#define XST_UART_BAUD_RANGE         1056


/************************ IIC statuses 1076 - 1100 ***************************/

#define XST_IIC_SELFTEST_FAILED         1076	/* self test failed            */
#define XST_IIC_BUS_BUSY                1077	/* bus found busy              */
#define XST_IIC_GENERAL_CALL_ADDRESS    1078	/* mastersend attempted with   */
					     /* general call address        */
#define XST_IIC_STAND_REG_RESET_ERROR   1079	/* A non parameterizable reg   */
					     /* value after reset not valid */
#define XST_IIC_TX_FIFO_REG_RESET_ERROR 1080	/* Tx fifo included in design  */
					     /* value after reset not valid */
#define XST_IIC_RX_FIFO_REG_RESET_ERROR 1081	/* Rx fifo included in design  */
					     /* value after reset not valid */
#define XST_IIC_TBA_REG_RESET_ERROR     1082	/* 10 bit addr incl in design  */
					     /* value after reset not valid */
#define XST_IIC_CR_READBACK_ERROR       1083	/* Read of the control register */
					     /* didn't return value written */
#define XST_IIC_DTR_READBACK_ERROR      1084	/* Read of the data Tx reg     */
					     /* didn't return value written */
#define XST_IIC_DRR_READBACK_ERROR      1085	/* Read of the data Receive reg */
					     /* didn't return value written */
#define XST_IIC_ADR_READBACK_ERROR      1086	/* Read of the data Tx reg     */
					     /* didn't return value written */
#define XST_IIC_TBA_READBACK_ERROR      1087	/* Read of the 10 bit addr reg */
					     /* didn't return written value */
#define XST_IIC_NOT_SLAVE               1088	/* The device isn't a slave    */

/*********************** ATMC statuses 1101 - 1125 ***************************/

#define XST_ATMC_ERROR_COUNT_MAX    1101	/* the error counters in the ATM
						   controller hit the max value
						   which requires the statistics
						   to be cleared */

/*********************** Flash statuses 1126 - 1150 **************************/

#define XST_FLASH_BUSY                1126	/* Flash is erasing or programming
						 */
#define XST_FLASH_READY               1127	/* Flash is ready for commands */
#define XST_FLASH_ERROR               1128	/* Flash had detected an internal
						   error. Use XFlash_DeviceControl
						   to retrieve device specific codes
						 */
#define XST_FLASH_ERASE_SUSPENDED     1129	/* Flash is in suspended erase state
						 */
#define XST_FLASH_WRITE_SUSPENDED     1130	/* Flash is in suspended write state
						 */
#define XST_FLASH_PART_NOT_SUPPORTED  1131	/* Flash type not supported by
						   driver */
#define XST_FLASH_NOT_SUPPORTED       1132	/* Operation not supported */
#define XST_FLASH_TOO_MANY_REGIONS    1133	/* Too many erase regions */
#define XST_FLASH_TIMEOUT_ERROR       1134	/* Programming or erase operation
						   aborted due to a timeout */
#define XST_FLASH_ADDRESS_ERROR       1135	/* Accessed flash outside its
						   addressible range */
#define XST_FLASH_ALIGNMENT_ERROR     1136	/* Write alignment error */
#define XST_FLASH_BLOCKING_CALL_ERROR 1137	/* Couldn't return immediately from
						   write/erase function with
						   XFL_NON_BLOCKING_WRITE/ERASE
						   option cleared */
#define XST_FLASH_CFI_QUERY_ERROR     1138	/* Failed to query the device */

/*********************** SPI statuses 1151 - 1175 ****************************/

#define XST_SPI_MODE_FAULT          1151	/* master was selected as slave */
#define XST_SPI_TRANSFER_DONE       1152	/* data transfer is complete */
#define XST_SPI_TRANSMIT_UNDERRUN   1153	/* slave underruns transmit register */
#define XST_SPI_RECEIVE_OVERRUN     1154	/* device overruns receive register */
#define XST_SPI_NO_SLAVE            1155	/* no slave has been selected yet */
#define XST_SPI_TOO_MANY_SLAVES     1156	/* more than one slave is being
						 * selected */
#define XST_SPI_NOT_MASTER          1157	/* operation is valid only as master */
#define XST_SPI_SLAVE_ONLY          1158	/* device is configured as slave-only
						 */
#define XST_SPI_SLAVE_MODE_FAULT    1159	/* slave was selected while disabled */

/********************** OPB Arbiter statuses 1176 - 1200 *********************/

#define XST_OPBARB_INVALID_PRIORITY  1176	/* the priority registers have either
						 * one master assigned to two or more
						 * priorities, or one master not
						 * assigned to any priority
						 */
#define XST_OPBARB_NOT_SUSPENDED     1177	/* an attempt was made to modify the
						 * priority levels without first
						 * suspending the use of priority
						 * levels
						 */
#define XST_OPBARB_PARK_NOT_ENABLED  1178	/* bus parking by id was enabled but
						 * bus parking was not enabled
						 */
#define XST_OPBARB_NOT_FIXED_PRIORITY 1179	/* the arbiter must be in fixed
						 * priority mode to allow the
						 * priorities to be changed
						 */

/************************ Intc statuses 1201 - 1225 **************************/

#define XST_INTC_FAIL_SELFTEST      1201	/* self test failed */
#define XST_INTC_CONNECT_ERROR      1202	/* interrupt already in use */

/********************** TmrCtr statuses 1226 - 1250 **************************/

#define XST_TMRCTR_TIMER_FAILED     1226	/* self test failed */

/********************** WdtTb statuses 1251 - 1275 ***************************/

#define XST_WDTTB_TIMER_FAILED      1251

/********************** PlbArb statuses 1276 - 1300 **************************/

#define XST_PLBARB_FAIL_SELFTEST    1276

/********************** Plb2Opb statuses 1301 - 1325 *************************/

#define XST_PLB2OPB_FAIL_SELFTEST   1301

/********************** Opb2Plb statuses 1326 - 1350 *************************/

#define XST_OPB2PLB_FAIL_SELFTEST   1326

/********************** SysAce statuses 1351 - 1360 **************************/

#define XST_SYSACE_NO_LOCK          1351	/* No MPU lock has been granted */

/********************** PCI Bridge statuses 1361 - 1375 **********************/

#define XST_PCI_INVALID_ADDRESS     1361

/********************** FlexRay constants 1400 - 1409 *************************/

#define XST_FR_TX_ERROR			1400
#define XST_FR_TX_BUSY			1401
#define XST_FR_BUF_LOCKED		1402
#define XST_FR_NO_BUF			1403

/****************** USB constants 1410 - 1420  *******************************/

#define XST_USB_ALREADY_CONFIGURED	1410
#define XST_USB_BUF_ALIGN_ERROR		1411
#define XST_USB_NO_DESC_AVAILABLE	1412
#define XST_USB_BUF_TOO_BIG		1413
#define XST_USB_NO_BUF			1414

/****************** HWICAP constants 1421 - 1430  *****************************/

#define XST_HWICAP_WRITE_DONE		1421

/**************************** Type Definitions *******************************/

typedef int XStatus;

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
