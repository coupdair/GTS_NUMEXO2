#include <stdio.h>
#include "xspi.h"
#include "xspi_i.h"
#include "xspi_l.h"

XSPI_Config *XSPI_LookupConfig(unsigned short DeviceId)
{
  XSPI_Config *CfgPtr = NULL;

  if (XSPI_ConfigTable[0].DeviceId == DeviceId)
  {
    CfgPtr = &XSPI_ConfigTable[0];
  }

  return CfgPtr;
}

int XSPI_Initialize(XSPI *InstancePtr, unsigned short DeviceId)
{
  XSPI_Config *CfgPtr;

//  XASSERT_NONVOID(InstancePtr != NULL);

  CfgPtr = XSPI_LookupConfig(DeviceId);

  if (CfgPtr == (XSPI_Config*)NULL) return XST_DEVICE_NOT_FOUND;

  InstancePtr->BaseAddress = CfgPtr->BaseAddress;

  XSPI_ClearStats(InstancePtr);

  return XST_SUCCESS;  
}

void XSPI_Reset(XSPI *InstancePtr)
{
  return;
}

int XSPI_SelfTest(XSPI *InstancePtr)
{
  return XST_SUCCESS;
}

int XSPI_Start(XSPI *InstancePtr)
{
  return XST_SUCCESS;
}

int XSPI_Stop(XSPI *InstancePtr)
{
  return XST_SUCCESS;
}

void XSPI_GetStats(XSPI *InstancePtr, XSPI_Stats *Stats)
{
  *Stats = InstancePtr->Stats;
}

void XSPI_ClearStats(XSPI *InstancePtr)
{
  int i;
  unsigned char *p = (unsigned char *)(&InstancePtr->Stats);

  for(i=0; i<sizeof(XSPI_Stats); i++) p[i]=0;
}

void XSPI_PrintCr(XSPI *InstancePtr)
{
  void *addr;
  XSPI_cr_t *cr;
  
  addr = InstancePtr->BaseAddress + XSPI_CR_OFFSET;
  printf("reg 0x%08X\n",*(unsigned int *)(addr));
  cr = (XSPI_cr_t *)(addr);
  printf("%u : Master Transaction Inhibit\n", cr->MasterTransactionInhibit);
  printf("%u : Manual Slave Select Assertion Enable\n",
         cr->ManualSlaveSelectAssertionEnable);
  printf("%u : Rx FIFO Reset\n", cr->RxFifoReset);
  printf("%u : Tx FIFO Reset\n", cr->TxFifoReset);
  printf("%u : Clock Phase\n", cr->Cpha);
  printf("%u : Clock Polarity\n", cr->Cpol);
  printf("%u : Master\n", cr->Master);
  printf("%u : SPI System Enable\n", cr->Spe);
  printf("%u : Local Loopback Mode\n", cr->Loop);
  return;
}

void XSPI_PrintSr(XSPI *InstancePtr)
{
  void *addr;
  XSPI_sr_t *sr;
  
  addr = InstancePtr->BaseAddress + XSPI_SR_OFFSET;
  sr = (XSPI_sr_t *)(addr);
  printf("reg 0x%08X\n",*(unsigned int *)(addr));
  printf("%u : Mode Fault Error\n", sr->Modf);
  printf("%u : Tx FIFO full\n", sr->TxFull);
  printf("%u : Tx FIFO empty\n", sr->TxEmpty);
  printf("%u : Rx FIFO full\n", sr->RxFull);
  printf("%u : Rx FIFO empty\n", sr->RxEmpty);
  return;
}

int XSPI_Init(XSPI *InstancePtr)
{
  void *addr;
  XSPI_cr_t *cr;
  
  addr = InstancePtr->BaseAddress + XSPI_CR_OFFSET;
  cr = (XSPI_cr_t *)(addr);
  cr->MasterTransactionInhibit = 0;
  cr->ManualSlaveSelectAssertionEnable = 1;
  cr->RxFifoReset = 1;
  cr->TxFifoReset = 1;
  cr->Cpha = 1; /* configuration for TDC */
  cr->Cpol = 0; /* configuration for TDC */
  cr->Master = 1; 
  cr->Spe = 1;
  cr->Loop = 0;
  cr->RxFifoReset = 0;
  cr->TxFifoReset = 0;
  return XST_SUCCESS;
}

