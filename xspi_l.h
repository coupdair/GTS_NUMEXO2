#ifndef XSPI_L_H
#define XSPI_L_H

#define XPAR_SPI_IF_BASEADDR 0x60020000
#define XPAR_SPI_IF_HIGHADDR 0x6002FFFF
#define XPAR_SPI_IF_DEVICE_ID 0
#define XPAR_SPI_IF_FIFO_EXIST 1
#define XPAR_SPI_IF_SPI_SLAVE_ONLY 0
#define XPAR_SPI_IF_NUM_SS_BITS 1

#define XSPI_IPIFR_OFFSET  0x40
#define XSPI_CR_OFFSET 	   0x60
#define XSPI_SR_OFFSET 	   0x64
#define XSPI_DTR_OFFSET	   0x68
#define XSPI_DRR_OFFSET    0x6C
#define XSPI_SSR_OFFSET	   0x70

/* See opb_spi.pdf for more details */

typedef struct 
{
	unsigned int 		                      : 23;
	unsigned int MasterTransactionInhibit 	      : 1;
	unsigned int ManualSlaveSelectAssertionEnable  : 1;
        unsigned int RxFifoReset		              : 1;
	unsigned int TxFifoReset		              : 1;
	unsigned int Cpha 	                      : 1; /* Clock Phase */
	unsigned int Cpol 			      : 1; /* Clock Polarity */
	unsigned int Master 			      : 1;
	unsigned int Spe			              : 1; /* SPI System Enable */
	unsigned int Loop 			      : 1; /* Local Loopback mode */
} XSPI_cr_t;


typedef struct
{
	unsigned int     			      : 27;
	unsigned int Modf 			      : 1;	/* Mode Fault Error Flag */
	unsigned int TxFull 			      : 1;
	unsigned int TxEmpty			      : 1;
	unsigned int RxFull 			      : 1;
	unsigned int RxEmpty			      : 1;
} XSPI_sr_t;


typedef struct
{
	unsigned int     		: 24;
	unsigned int  TxData             : 8;
} __attribute__ ((packed)) XSPI_dtr_t;

typedef struct
{
	unsigned int      		: 24;
	unsigned int  RxData		: 8;
} __attribute__ ((packed)) XSPI_drr_t;

#endif
