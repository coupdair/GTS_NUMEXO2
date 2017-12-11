/**
 * \file gtsMain.cpp
 * \brief gts EPICS example program (i.e. template)
 * \author Sebastien COUDERT
**/

#define VERSION "v0.1.2e"

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
   "gts: EPICS example program, UDP device server\
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

//! CLI option parse and run iocsh
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

//!

  XSPI_ConfigTable[0].BaseAddress = (void *)(&XSPIScratch[0]);

  XMMRegs_Initialize(&XMMRegsDriver, 0);

  XSPI_Initialize(&XSPIDriver, 0);

//!

  XMMRegs_Reg_Init(&XMMRegsDriver);

  c = (XMMRegs_lmk_pll_ctrl *)(XMMRegsDriver.BaseAddress + XMMR_LMK_PLL_CTRL_OFFSET);

  c->hw_ctrl = 1;
  c->hw_init_conf = 1;

  XMMRegs_RocketIO_Gtx_Reset(&XMMRegsDriver);

#endif //_X86_64_

//! EPICS or UDP service
if(arguments.epicsFlow)
{//EPICS
  printf("GTS server 4 EPICS: not implemented yet !\n");
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

//! - initialise and start \b client socket

  gtsCliSock = clientSetup();

//! - initialise and start \b server socket

  logAnswer();

  udpServer();

#endif //_X86_64_
  }//PPC arch
}//UDP
  return(0);
}//main

