//GTS_server
/*-----------------------------------------------------------------------
File : gtsServer.c
Description : NUMEXO2 GTS server (GANIL)
Authors : Frederic SAILLANT, Sebastien COUDERT
-----------------------------------------------------------------------*/

//! version of \b old \c GTS_server
/**
 * \c \b old GTS_server code version, should be changed by the developper in this \c gtsServer.c C file
**/
#define VERSION "v1.0.3"

/*Additional documentation for the generation of the reference page (using doxygen)*/
/**
\page old OLD NUMEXO2 GTS server (UDP)
*
* this is \b old program (for \c main() code) and its documentation:
*
* table of content:
* \li \ref sectionOldCodeHistory
* \li \ref sectionOldCommandLine
* \li \ref sectionOldServerCommands
* \li \ref sectionOldGTSserverDocumentation
* \li \ref sectionOldDoxygenSyntax
*
* \section sectionOldCodeHistory Modifications
*

    S. Coudert 24/07/2015
     - add version support (option -v)
     - add help    support (option -h)

    S. Coudert 12/02/2016
     - stable v1.0.0 for production

    S. Coudert 15/05/2017
     - add server commands to documentation

    S. Coudert 03/07/2017
     - move function from gtsServer to gtsMisc (for future EPICS)

    S. Coudert 15/12/2017
     - setup documentation to forward to current main program (running both UDP and EPICS)

    F. Saillant 12/01/2017
     - add loopbackSet

*
* \section sectionOldCommandLine command line options
*
* - GTS server (UDP only; old)
*
* \verbinclude "gtsServer.help.output"
*
* \section sectionOldServerCommands server commands
*
* \verbinclude "gtsServer_command_UDP.txt"
*
* \section sectionOldGTSserverDocumentation documentation outline
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
* \section sectionOldDoxygenSyntax make documentation using Doxygen syntax
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

//global Misc
extern XMMRegs XMMRegsDriver;

extern XSPI XSPIDriver;

extern XMMRegs_Config XMMRegs_ConfigTable[];

extern XSPI_Config XSPI_ConfigTable[];

extern unsigned int Scratch[];

extern unsigned int XSPIScratch[];

extern unsigned int cardNumber;

//other globals
extern int gtsCliSock;

extern int clientSetup (void);

extern void udpServer (void);

//! \c old main, but still compiled for object files ! For current one see <a href="gtsMain_8cpp.html">gtsMain.cpp</a> \see gtsMain.cpp
int main (int argc, char *argv[])
{
  //command line options (C/unistd)
  int opt=0;
//! - command line options
  while( (opt=getopt(argc, argv, "vh")) !=-1)
  {
    switch(opt)
    {
    case 'h':
      printf("%s (%s - %s %s)\n",argv[0],VERSION,__DATE__,__TIME__);
      printf("description:\n");
      printf("\told GTS embedded server for NUMEXO2 (UDP only).\n");
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

//! - initialize globals
  initialize();

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
}//main

