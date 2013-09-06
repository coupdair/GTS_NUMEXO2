#ifndef XSPI_MMREGS_TDCGP2_H
#define XSPI_MMREGS_TDCGP2_H

#include "xspi.h"
#include "xspi_tdcgp2.h"
#include "xmmregs_i.h"

#define TDC_DEBUG            1
#define NO_TDC_DEBUG         0

#define MEAS1_NON_CALIBRATED 0
#define MEAS1                1
#define MEAS2                2
#define CALIBRATION          3

/* #define WAIT_IN_MICROSEC     8 */
#define WAIT_IN_MICROSEC 8

/* low level */
extern int xtdc_CommunicationSet(XMMRegs *InstancePtr, int debug_mode, unsigned int delay);
extern tdcgp2_reg_t xtdc_Configure(XSPI *InstanceSPI, int measure);
extern int xtdc_NewMeasure(XSPI *InstanceSPI, tdcgp2_reg_t *r1, int measure);
extern int xtdc_DoOneSweep(XMMRegs *InstancePtr, int debug);
extern int xtdc_MeasureRead(XSPI *InstanceSPI, int measure);

/* high level */

extern int xtdc_Meas1NonCalibrated_Measure(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes);
extern int xtdc_Meas1Calibrated_Measure(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes);
extern int xtdc_Meas2Calibrated_Measure(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes);
extern int xtdc_Meas1Calibrated_Calibrate(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes);
extern int xtdc_Meas2Calibrated_Calibrate(XSPI *InstanceSPI, XMMRegs *InstancePtr, unsigned int delay, unsigned int nmes);

#endif
