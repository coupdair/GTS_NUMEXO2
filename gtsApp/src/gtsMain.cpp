/**
 * \file gtsMain.cpp
 * \brief GTS server using UDP or EPICS (from \c gtsServer.c)
 * \author Sebastien COUDERT
**/


//! version of \c GTS_server, i.e. NUMEXO2 GTS server (GANIL)
/**
 * \c GTS_server code version, should be changed by the developper in this \c gtsMain.c C++ file
**/
#define VERSION "v0.2.3f"

/*Additional documentation for the generation of the reference page (using doxygen)*/
/**
\mainpage NUMEXO2 GTS server
* table of content:
* \li \ref sectionCodeHistory
* \li \ref sectionCommandLine
* \li \ref sectionServerCommands
* <ul>
* <li> \ref subsectionSCUDP </li>
* <li> \ref subsectionSCEPICS </li>
* <li> \ref subsectionSCUDPvsEPICS </li>
* <li> \ref subsectionSCEPICSvsAGATA </li>
* </ul>
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

    S. Coudert 03/07/2017
     - move function from gtsServer to gtsMisc (for future EPICS)

    S. Coudert 15/12/2017
     - add EPICS functions (record: .DESC, .A, .B, .C upto .D)

* \section sectionCommandLine command line options
*
* - GTS server with EPICS+UDP
*
* \verbinclude "gts.help.output"
*
* - \b old GTS server (UDP only)
*
* \verbinclude "gtsServer.help.output"
*
* \section sectionServerCommands server commands
*
* \subsection subsectionSCUDP UDP list
*
* \verbinclude "gtsServer_command_UDP.txt"
*
* \subsection subsectionSCEPICS EPICS list
*
* - record: DESC, .A, .B, .C upto .D
*
* \verbinclude "gtsServer_command_EPICS.txt"
*
* \subsection subsectionSCUDPvsEPICS UDP vs EPICS command table
*
* \verbinclude "gtsServer_command.txt"
*
* \subsection subsectionSCEPICSvsAGATA EPICS vs AGATA table (development progressing)
*
* \verbinclude "gtsServer_command_EPICSvsAGATA.txt"
*
* \section sectionGTSserverDocumentation documentation outline
* This is the reference documentation of 
* <a href="http://wiki.ganil.fr/gap/">NUMEXO2 GTS server</a>, 
* from the <a href="http://www.ganil-spiral2.eu">GANIL</a>.\n\n
* The main function is in <a href="gtsMain_8cpp.html">gtsMain.cpp</a> source file.\n\n
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

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"
#include "envDefs.h" //envPrtConfigParam
//ArgP
#include <error.h>
#include <argp.h>

#include <string>

//GTS device
#ifndef _X86_64_

extern "C"
{
#include "../../gts.h"
void XMMRegs_RocketIO_Gtx_Reset(XMMRegs *InstancePtr);
int clientSetup (void);
void udpServer (void);
}

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

#endif //_X86_64_
//}GTS device

//Program option/documentation
//{argp
//! [argp] version of program
const char *argp_program_version=VERSION;
//! [argp] author of program
const char *argp_program_bug_address="sebastien.coudert@ganil.fr";
//! [argp] documentation of program
static char doc[]=
   "gts: GTS server using EPICS or UDP\
\n  gts."VERSION"\
\n\
examples:\n\
  gts --help\n\
  gts --epics -c st.cmd\n\
  gts -v\n\
  gts -V\n\
  gts --usage";

//! [argp] A description of the arguments
static char args_doc[] = "";

//! [argp] The options and its description
static struct argp_option options[]=
{
  {"verbose",  'v', 0, 0,        "Produce verbose output" },
  {"epics",    'e', 0, 0,        "Use EPICS workflow, otherwise UDP by default" },
  {"command",  'c', "st.cmd", 0, "EPICS: execute IOC command file, e.g. st.cmd, before interactive shell" },
//default options
  { 0 }
};//options (CLI)

//! [argp] Used by main to communicate with parse_option
struct arguments
{
  //! verbose mode
  int verbose;
  //! EPICS mode
  int epicsFlow;
  //! command file name
  char* cmd_fname;
};//arguments (CLI)

//! [argp] Parse a single option
static error_t
parse_option(int key, char *arg, struct argp_state *state)
{
  //Get the input argument from argp_parse
  struct arguments *arguments=(struct arguments *)(state->input);
  switch (key)
  {
    case 'v':
      arguments->verbose=1;
      break;
    case 'e':
      arguments->epicsFlow=1;
      break;
    case 'c':
      arguments->cmd_fname=arg;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }//switch
  return 0;
}//parse_option


//! [argp] print argument values
void print_args(struct arguments *p_arguments)
{
  printf (".verbose=%s\n.epicsFlow=%s\n.command_file_name=\"%s\"\n"
  , p_arguments->verbose?"yes":"no"
  , p_arguments->epicsFlow?"yes":"no"
  , p_arguments->cmd_fname
  );
}//print_args

//! [argp] setup argp parser
static struct argp argp = { options, parse_option, args_doc, doc };

//}argp

//! CLI option parse, then run iocsh or UDP server
int main(int argc,char *argv[])
{
  //CLI arguments
  struct arguments arguments;
  arguments.verbose=0;
  arguments.epicsFlow=0;
  arguments.cmd_fname="st.cmd";

//! - print default option values (static)
  if(0)
  {
    printf("default values:\n");
    print_args(&arguments);
  }//print default option values

//! - Parse arguments (see parse_option) and eventually show usage, help or version and exit for these lasts
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  ///print option values
  if(arguments.verbose)
  {
    printf("command line option values:\n");
    print_args(&arguments);
  }//print default option values

  //! fake device if not on NumExo2 device (i.e. PPC CPU), e.g. x86; usage: if(fake_device) printf("x86"); else printf("PPC");
  int fake_device;
  {//arch
  //check CPU architecture
  const char*arch=      envGetConfigParamPtr(&EPICS_BUILD_TARGET_ARCH);
  if(arguments.verbose) envPrtConfigParam   (&EPICS_BUILD_TARGET_ARCH);
  fake_device=strcmp("linux-ppc",arch);
  }//arch

//! module access
#ifndef _X86_64_
//! - GTS id from IP address
  XMMRegs_lmk_pll_ctrl *c;
  setCardNumber();
  printf("card number : %u\n", cardNumber);

//! - initialize globals
  initialize();

//! - initialize XSPI
  XSPI_ConfigTable[0].BaseAddress = (void *)(&XSPIScratch[0]);
  XMMRegs_Initialize(&XMMRegsDriver, 0);
  XSPI_Initialize(&XSPIDriver, 0);

//! - initialize XMMRegs
  XMMRegs_Reg_Init(&XMMRegsDriver);
  c = (XMMRegs_lmk_pll_ctrl *)(XMMRegsDriver.BaseAddress + XMMR_LMK_PLL_CTRL_OFFSET);
  c->hw_ctrl = 1;
  c->hw_init_conf = 1;
  XMMRegs_RocketIO_Gtx_Reset(&XMMRegsDriver);

//! - initialise and start \b client socket
  gtsCliSock = clientSetup();
  logAnswer();

#endif //_X86_64_

//! EPICS or UDP service
if(arguments.epicsFlow)
{//EPICS
  printf("GTS server 4 EPICS: not fully implemented yet !\n  for complete list, type:\n  epics> dbl\n");
  //batch IOC shell
  if(argc>=2)
  {
    iocsh(arguments.cmd_fname);
    epicsThreadSleep(.2);
  }//batch
  //interactive IOC shell
  iocsh(NULL);
  epicsExit(0);
}//EPICS
else
{//UDP
  if(fake_device)
  {//other arch, e.g. "g++ -D_X86_64_"
    printf("UDP server: fake is not implemented yet !\n");
  }//other arch
  else
  {//PPC arch, i.e. "g++ -D_PPC_"
#ifndef _X86_64_

    printf("UDP server: work in progress (test on device needed).\n");

//! - initialise and start \b server socket
  udpServer();

#endif //_X86_64_
  }//PPC arch
}//UDP
  return(0);
}//main

