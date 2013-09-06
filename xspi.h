#ifndef XSPI_H
#define XSPI_H

#include "xstatus.h"
#include "xspi_l.h"

typedef struct {
	unsigned short DeviceId;		/**< Unique ID  of device */
        void *BaseAddress;
        unsigned short FifoExist;
        unsigned short SpiSlave;
        unsigned short NumSSBits;
} XSPI_Config;
                    
typedef struct {
   unsigned int ReceivedBytes;
   unsigned int SentBytes;
   unsigned int MissingTargets;
} XSPI_Stats;

typedef struct {
   XSPI_Stats Stats;
   void *BaseAddress;
} XSPI;
 
/* standard API */
extern int XSPI_Initialize(XSPI *InstancePtr, unsigned short DeviceId);
extern void XSPI_Reset(XSPI *InstancePtr);
extern int XSPI_SelfTest(XSPI *InstancePtr);
extern int XSPI_Start(XSPI *InstancePtr);
extern int XSPI_Stop(XSPI *InstancePtr);
extern void XSPI_GetStats(XSPI *InstancePtr, XSPI_Stats *Stats);
extern void XSPI_ClearStats(XSPI *InstancePtr);


/* Custom API */

extern void XSPI_PrintCr(XSPI *InstancePtr);
extern void XSPI_PrintSr(XSPI *InstancePtr);
extern int  XSPI_Init(XSPI *InstancePtr);

/***********************************************/
/*  TDCGP2 APIs                             */
/***********************************************/

extern int XSPI_tdcgp2_InitSPI(XSPI *InstancePtr);
extern unsigned int XSPI_tdcgp2_RegRead(XSPI *InstancePtr, unsigned int);
extern int XSPI_tdcgp2_RegWrite(XSPI *InstancePtr, unsigned int, unsigned int);
extern int XSPI_tdcgp2_PowerOnReset(XSPI *InstancePtr);
extern int XSPI_tdcgp2_InitDataReg(XSPI *InstancePtr);
extern int XSPI_tdcgp2_CalResonator(XSPI *InstancePtr);
extern int XSPI_tdcgp2_CalTDC(XSPI *InstancePtr);

#endif
