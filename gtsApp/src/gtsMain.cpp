/**
 * \file gtsMain.cpp
 * \brief gts EPICS example program (i.e. template)
 * \author Sebastien COUDERT
**/

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsExit.h"
#include "epicsThread.h"
#include "iocsh.h"

//ArgP
#include <error.h>
#include <argp.h>

#include <string>

#define VERSION "v0.0.2"

//Program option/documentation
//{argp
//! [argp] version of program
const char *argp_program_version=VERSION;
//! [argp] author of program
const char *argp_program_bug_address="sebastien.coudert@ganil.fr";
//! [argp] documentation of program
static char doc[]=
   "gts: EPICS example program\
\n  gts."VERSION"\
\n\
examples:\n\
  gts --help\n\
  gts -e st.cmd\n\
  gts -v\n\
  gts -V\n\
  gts --usage";

//! [argp] A description of the arguments
static char args_doc[] = "";

//! [argp] The options and its description
static struct argp_option options[]=
{
  {"verbose",  'v', 0, 0,        "Produce verbose output" },
  {"command",  'e', "st.cmd", 0, "Execute IOC command file, e.g. st.cmd, before interactive shell" },
//default options
  { 0 }
};//options (CLI)

//! [argp] Used by main to communicate with parse_option
struct arguments
{
  //! verbose mode
  int verbose;
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
  printf (".verbose=%s\ncommand_file_name=%s\n"
  , p_arguments->verbose?"yes":"no"
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

//! start EPICS service
  //batch IOC shell
  if(argc>=2)
  {
    iocsh(arguments.cmd_fname);
    epicsThreadSleep(.2);
  }//batch
  //interactive IOC shell
  iocsh(NULL);
  epicsExit(0);

  return(0);
}//main

