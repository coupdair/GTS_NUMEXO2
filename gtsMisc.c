//GTS_misc
/*-----------------------------------------------------------------------
File : gtsMisc.c
Description : NUMEXO2 GTS server (GANIL)
Authors : Frederic SAILLANT, 
-----------------------------------------------------------------------*/

#include "gts.h"

XMMRegs XMMRegsDriver;

XSPI XSPIDriver;

XMMRegs_Config XMMRegs_ConfigTable[] = {{0,0}};
/*
XSPI_Config XSPI_ConfigTable[] =
{
	{
		XPAR_SPI_IF_DEVICE_ID,
		XPAR_SPI_IF_BASEADDR,
                XPAR_SPI_IF_FIFO_EXIST,
                XPAR_SPI_IF_SPI_SLAVE_ONLY,
                XPAR_SPI_IF_NUM_SS_BITS
	}
};
*/

XSPI_Config XSPI_ConfigTable[] =
{
	{
		XPAR_SPI_IF_DEVICE_ID,
		0,
                XPAR_SPI_IF_FIFO_EXIST,
                XPAR_SPI_IF_SPI_SLAVE_ONLY,
                XPAR_SPI_IF_NUM_SS_BITS
	}
};

unsigned int Scratch[128];

unsigned int XSPIScratch[0x4000];

unsigned int cardNumber = 225;

void setCardNumber(void)
{//IP address
  char ip[INET_ADDRSTRLEN];
  struct ifconf ifconf;
  struct ifreq ifreqs[2];
  int fd;

  memset( (void *)&ifconf, 0, sizeof(ifconf) );

  ifconf.ifc_buf = (char *)ifreqs;
  ifconf.ifc_len = sizeof(ifreqs);

  fd = socket( AF_INET, SOCK_STREAM, 0 );

  ioctl( fd, SIOCGIFCONF, (char *)&ifconf );

  close(fd);

  inet_ntop( AF_INET, &(((struct sockaddr_in *)&(ifreqs[1].ifr_addr))->sin_addr), ip, INET_ADDRSTRLEN );

  printf("\nIP address : %s\n", ip);

  cardNumber = ((struct sockaddr_in *)&(ifreqs[1].ifr_addr))->sin_addr.s_addr & 0xFF;
}//IP address

//! initialize globals
void initialize(void)
{
//! - allocate strings
  FIRST_ARG = last_sentence_serial;

  memset(global_log,0,MAX_CHAR_LOG);

  memset(last_sentence_log,0,MAX_CHAR_LOG);

  memset(last_sentence_serial,0,MAX_CHAR_SERIAL);

  memset(global_output,0,MAX_CHAR_OUTPUT);

  memset(last_sentence_output,0,MAX_CHAR_OUTPUT);

  memset(file_name_output,0,MAX_CHAR_FILE_OUTPUT);

//  dbgTermSet();
  dbgNetSet();

  memset( (void *)&XMMRegsDriver, 0, sizeof( XMMRegs ) );

  memset( (void *)&XSPIDriver, 0, sizeof( XSPI ) );

//  XMMRegs_ConfigTable[0].BaseAddress = (void *)(&Scratch[0]);

//! - memory map
  {//mmap

  int fd;

  fd = open("/dev/memory", O_RDWR);

  if (fd == -1)
  {
    perror("open(\"/dev/memory\")");

    return -1;
  }

  XMMRegs_ConfigTable[0].BaseAddress = (void *)mmap(NULL, 0x400, O_RDWR, MAP_SHARED, fd, 0x8181C000);
  close(fd);

  if (XMMRegs_ConfigTable[0].BaseAddress == MAP_FAILED)
  {
    perror("mmap");

    return -1;
  }
  }//mmap
}//initialize

