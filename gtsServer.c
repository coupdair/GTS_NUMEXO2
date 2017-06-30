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
#define VERSION "v1.0.1e"

/*Additional documentation for the generation of the reference page (using doxygen)*/
/**
\mainpage NUMEXO2 GTS server
* table of content:
* \li \ref sectionCodeHistory
* \li \ref sectionCommandLine
* \li \ref sectionServerCommands
* \li \ref sectionGTSserverDocumentation
* \li \ref sectionDoxygenSyntax
*
* \section sectionCodeHistory Modifications
*

    S. Coudert 24/07/2015
     - add version support (option -v)
     - add help    support (option -h)

    S. Coudert 12/02/2016
     - stable v1.0.0 for production

    S. Coudert 15/05/2017
     - add server commands to documentation

* \section sectionCommandLine command line options
*
* \verbinclude "gtsServer.help.output"
*
* \section sectionServerCommands server commands
*
* \verbinclude "gtsServer_command.txt"
*
* \section sectionGTSserverDocumentation documentation outline
* This is the reference documentation of 
* <a href="http://wiki.ganil.fr/gap/">NUMEXO2 GTS server</a>, 
* from the <a href="http://www.ganil-spiral2.eu">GANIL</a>.\n\n
* The main function is in <a href="gtsServer_8c.html">gtsServer.c</a> source file.\n\n
* This documentation has been automatically generated from the sources,
* using the tool <a href="http://www.doxygen.org">doxygen</a>. It should be readed as HTML, LaTex and man page.\n
* It contains both
* \li a detailed description of all classes and functions
* \li TODO: a user guide (cf. \ref pages.html "related pages")
*
* that as been documented within the sources.
*
* \section sectionDoxygenSyntax make documentation using Doxygen syntax
* Each function in the source code should be commented using \b doxygen \b syntax in the same file.
* The documentation need to be written before the function.
* The basic syntax is presented in this part.
* \verbinclude "doxygen.example1.txt"
*
* Two kind of comments are needed for \b declaration and \b explanation \b parts of the function:
* Standart documentation should the following (\b sample of code documentation):
* \verbinclude "doxygen.example2.txt"
*
* In both declaration and explanation part, \b writting and \b highlithing syntax can be the following:\n\n
*
* \li \c \\n a new line
* \li \c \\li a list (dot list)
*
* \li \c \\b bold style
* \li \c \\c code style
* \li \c \\e enhanced style (italic)
*
* For making \b shortcut please use:\n
* \li \c \\see to make a shortcut to a related function or variable
* \li \c \\link to make a shortcut to a file or a function
* \note this keyword needs to be closed using \c \\end*
*
* \li \c \\todo to add a thing to do in the list of <a href="todo.html">ToDo</a> for the whole program
*
* In explanation part, \b paragraph style can be the following:\n
* \li \c \\code for an example of the function use
* \li \c \\note to add a few notes
* \li \c \\attention for SOMETHING NOT FULLY DEFINED YET
* \li \c \\warning to give a few warning on the function
* \note these keywords need to be closed using \c \\end*
*
* \verbinclude "doxygen.example3.txt"
*
* Many other keywords are defined, so please read the documentation of <a href="http://www.doxygen.org/commands.html">doxygen</a>.
*
**/

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

void setCardNumber()
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

int main (int argc, char *argv[])
{
  //command line options (C/unistd)
//! \todo [low] use argp for CLI options (from libC)
  int opt=0;
//! - command line options
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

//! - GTS id from IP address
  XMMRegs_lmk_pll_ctrl *c;
  setCardNumber();
  printf("card number : %u\n", cardNumber);

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

//! - initialise and start \b client socket

  XSPI_ConfigTable[0].BaseAddress = (void *)(&XSPIScratch[0]);

  XMMRegs_Initialize(&XMMRegsDriver, 0);

  XSPI_Initialize(&XSPIDriver, 0);

  gtsCliSock = clientSetup();

//! - initialise and start \b server socket

  XMMRegs_Reg_Init(&XMMRegsDriver);

  c = (XMMRegs_lmk_pll_ctrl *)(XMMRegsDriver.BaseAddress + XMMR_LMK_PLL_CTRL_OFFSET);

  c->hw_ctrl = 1;
  c->hw_init_conf = 1;

  XMMRegs_RocketIO_Gtx_Reset(&XMMRegsDriver);

  logAnswer();

  udpServer();

  return 0;
}
