//GTS_server
/*-----------------------------------------------------------------------
File : gtsServer.c
Description : NUMEXO2 GTS server (GANIL)
Authors : Frederic SAILLANT, 
-----------------------------------------------------------------------*/

//! version of \c GTS_server, i.e. NUMEXO2 GTS server (GANIL)
/**
 * \c GTS_server code version, should be changed by the developper in this \c gtsServer.c C file
**/
#define VERSION "v0.0.1"

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

extern int gtsCliSock;

extern int clientSetup (void);

extern void udpServer (void);

int main (int argc, char *argv[])
{
  //command line options (C/unistd)
  int opt=0;
  while( (opt=getopt(argc, argv, "vh")) !=-1)
  {
    switch(opt)
    {
    case 'h':
      printf("%s (%s - %s %s)\n",argv[0],VERSION,__DATE__,__TIME__);
      printf("description:\n");
      printf("\tGTS embedded server for NUMEXO2.\n");
      printf("usage:\n");
      printf("\t%s\n",argv[0]);
      printf("\t%s -h\n",argv[0]);
      printf("\t%s -v\n",argv[0]);
      printf("options:\n");
      printf("\t-h: help\n");
      printf("\t-v: version\n");
      return 0;
    break;//help
    case 'v':
      printf("%s.%s\n",argv[0],VERSION);
      return 0;
    break;//version
    case '?':
      printf("unknown option: %s\n", optarg);
      return 1;
    break;
    }
  }//command line options (-.)

  char ip[INET_ADDRSTRLEN];
  struct ifconf ifconf;
  struct ifreq ifreqs[2];
  int fd;
  XMMRegs_lmk_pll_ctrl *c;

  memset( (void *)&ifconf, 0, sizeof(ifconf) );

  ifconf.ifc_buf = (char *)ifreqs;
  ifconf.ifc_len = sizeof(ifreqs);

  fd = socket( AF_INET, SOCK_STREAM, 0 );

  ioctl( fd, SIOCGIFCONF, (char *)&ifconf );

  close(fd);

  inet_ntop( AF_INET, &(((struct sockaddr_in *)&(ifreqs[1].ifr_addr))->sin_addr), ip, INET_ADDRSTRLEN );

  printf("\nIP address : %s\n", ip);

  cardNumber = ((struct sockaddr_in *)&(ifreqs[1].ifr_addr))->sin_addr.s_addr & 0xFF;

  printf("card number : %u\n", cardNumber);

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

  fd = open("/dev/memory", O_RDWR);

  if (fd == -1)
  {
    perror("open(\"/dev/memory\")");

    return -1;
  }

  XMMRegs_ConfigTable[0].BaseAddress = (void *)mmap(NULL, 0x400, O_RDWR, MAP_SHARED, fd, 0x8181C000);

  if (XMMRegs_ConfigTable[0].BaseAddress == MAP_FAILED)
  {
    perror("mmap");

    return -1;
  }

  XSPI_ConfigTable[0].BaseAddress = (void *)(&XSPIScratch[0]);

  XMMRegs_Initialize(&XMMRegsDriver, 0);

  XSPI_Initialize(&XSPIDriver, 0);

  gtsCliSock = clientSetup();

  XMMRegs_Reg_Init(&XMMRegsDriver);

  c = (XMMRegs_lmk_pll_ctrl *)(XMMRegsDriver.BaseAddress + XMMR_LMK_PLL_CTRL_OFFSET);

  c->hw_ctrl = 1;
  c->hw_init_conf = 1;

  XMMRegs_RocketIO_Gtx_Reset(&XMMRegsDriver);

  logAnswer();

  udpServer();

  close(fd);

  return 0;
}
