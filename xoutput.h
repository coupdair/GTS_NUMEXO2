#ifndef XOUTPUT_H
#define XOUTPUT_H
#include "xoutput_i.h"

/************************************************************

		user choices

************************************************************/


/* for highlevel driver files */
#define XTDC_OUTPUT               /* output of xtdc.c */

/************************************************************

		TPT_* DEFINITIONS
		
   (should not be touched except when creating a new file)

************************************************************/

/************* for xtdc files *******************/

#ifdef XTDC_OUTPUT
#  define TPT_XTDC(f, args...) {eprintf_output(FIRST_ARG_OUTPUT, f, ##args ); estrcpy_output();}
#else
#  define TPT_XTDC(f...) {}
#endif

#endif

