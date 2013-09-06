#ifndef XSPI_TDCGP2_H
#define XSPI_TDCGP2_H

#include "xspi_i.h"

#define TDCGP2_CMD_WRITE        0x80
#define TDCGP2_CMD_READ         0xB0
#define TDCGP2_CMD_INIT         0x70
#define TDCGP2_CMD_PORESET      0x50
#define TDCGP2_CMD_SCYC         0x01
#define TDCGP2_CMD_STEM         0x02
#define TDCGP2_CMD_SCRE         0x03
#define TDCGP2_CMD_SCTD         0x04
#define TDCGP2_ADD_MSK	        0x00000007

#define TDCGP2_REG0_DEFAULT 	0x00000668
#define TDCGP2_REG1_DEFAULT 	0x00554000
#define TDCGP2_REG2_DEFAULT 	0x00200000
#define TDCGP2_REG3_DEFAULT 	0x00180000
#define TDCGP2_REG4_DEFAULT 	0x00200000
#define TDCGP2_REG5_DEFAULT 	0x00000000
#define TDCGP2_WREG_NUMBER	6
#define TDCGP2_RREG_NUMBER	6

#define TDCGP2_SEND(v)  (*(unsigned int *)(XSPI_ConfigTable[0].BaseAddress+XSPI_DTR_OFFSET) = (v))
#define TDCGP2_RECV() 	(*(unsigned int *)(XSPI_ConfigTable[0].BaseAddress+XSPI_DRR_OFFSET))
#define TDCGP2_SELECT(i) (*(unsigned int *)(XSPI_ConfigTable[0].BaseAddress+XSPI_SSR_OFFSET) &= ~(1 << (i))) 
#define TDCGP2_UNSELECT(i) (*(unsigned int *)(XSPI_ConfigTable[0].BaseAddress+XSPI_SSR_OFFSET) |= (1 <<(i))) 

/* TDC REGISTERS */

typedef struct
{
	unsigned int			: 8;
	unsigned int  FireN 		: 4;
	unsigned int  DivFire 		: 4;
	unsigned int  CalResN 	        : 2;
	unsigned int  ClkHSDiv		: 2;
        unsigned int  StartClkHS	        : 2;
	unsigned int  PortN 		: 1;
	unsigned int  TCycle		: 1;
	unsigned int  NoFake		: 1;
	unsigned int  SelClkT 		: 1;
	unsigned int  Calibrate   	: 1;
	unsigned int  DisAutoCal  	: 1;
	unsigned int  MRange2 	        : 1;
	unsigned int  NegStop2		: 1;
	unsigned int  NegStop1		: 1;
	unsigned int  NegStart	        : 1;
} __attribute__ ((packed)) tdcgp2_reg0_t;

typedef struct
{
	unsigned int			: 8;
	unsigned int  Hit2 		: 4;
	unsigned int  Hit1 		: 4;
	unsigned int  EnFastInit 	: 1;
	unsigned int  sc			: 1;
	unsigned int  HitIn2	        : 3;
	unsigned int  HitIn1 		: 3;
	unsigned int   			: 8;
} __attribute__ ((packed)) tdcgp2_reg1_t;

typedef struct
{
	unsigned int			: 8;
	unsigned int  EnInt 		: 3;
	unsigned int  RfEdge2 		: 1;
	unsigned int  RfEdge1     	: 1;
	unsigned int  DelVal1		: 19;
} __attribute__ ((packed)) tdcgp2_reg2_t;

typedef struct
{
	unsigned int			: 8;
	unsigned int  sc 		: 2;
	unsigned int  EnErrVal 		: 1;
	unsigned int  SelTimoMr2 	: 2;
	unsigned int  DelVal2		: 19;
} __attribute__ ((packed)) tdcgp2_reg3_t;

typedef struct
{
	unsigned int			: 8;
	unsigned int  sc 		: 5;
	unsigned int  DelVal3		: 19;
} __attribute__ ((packed)) tdcgp2_reg4_t;

typedef struct
{
	unsigned int			: 8;
	unsigned int  ConfFire 	        : 3;
	unsigned int  EnStartnoise	: 1;
	unsigned int  DisPhasenoise      : 1;
	unsigned int  RepeatFire		: 3;
	unsigned int  PhaseFire		: 16;
} __attribute__ ((packed)) tdcgp2_reg5_t;


typedef union
{
	unsigned int                  uint;
	struct 
	{
		unsigned short	I;
		unsigned short  D;
	} __attribute__ ((packed)) C;
	struct 
	{
		unsigned short Meas;
		unsigned short nu;
	} __attribute__ ((packed)) NC;
} __attribute__ ((packed)) tdcgp2_res_t;

typedef struct
{
        unsigned int			  : 16;
	unsigned int			  : 3;
	unsigned int	ErrorShort	  : 1;
	unsigned int	ErrorOpen	  : 1;
	unsigned int	TimeoutPrecounter : 1;
	unsigned int	TimeoutTDC	  : 1;
	unsigned int  NHitsCh2		  : 3;
	unsigned int  NHitsCh1		  : 3;
	unsigned int  PointerResReg	  : 3;
} __attribute__ ((packed)) tdcgp2_stat_t;

typedef struct
{
	unsigned int			  : 24;
	unsigned int	Reg1		  : 8;
} __attribute__ ((packed)) tdcgp2_testr_t;


typedef union
{
	unsigned int  	        uint;
	tdcgp2_reg0_t 	r0;
	tdcgp2_reg1_t 	r1;
	tdcgp2_reg2_t 	r2;
	tdcgp2_reg3_t 	r3;
	tdcgp2_reg4_t 	r4;
	tdcgp2_reg5_t 	r5;
	tdcgp2_res_t 	res;
	tdcgp2_stat_t 	stat;
	tdcgp2_testr_t 	test;
} __attribute__ ((packed)) tdcgp2_reg_t;

/***********************************************/
/*  TDCGP2 APIs                                */
/***********************************************/

/* TDCGP2 configuration functions */

extern tdcgp2_reg_t XSPI_tdcgp2_Meas1NonCalibrated_Configure(XSPI *InstanceSPI);
extern tdcgp2_reg_t XSPI_tdcgp2_Meas1Calibrated_Configure(XSPI *InstanceSPI);
extern tdcgp2_reg_t XSPI_tdcgp2_Meas2Calibrated_Configure(XSPI *InstanceSPI);
extern tdcgp2_reg_t XSPI_tdcgp2_MeasCalibration_Configure(XSPI *InstanceSPI);


#endif
