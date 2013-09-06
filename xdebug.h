#ifndef XDEBUG_H
#define XDEBUG_H
#include "xdebug_i.h"

/************************************************************

		user choices

************************************************************/

/************************************************************

	          GLOBAL DEBUG LEVELS

    define the global debug levels for each set of drivers

************************************************************/

#define XHIGHLEVEL_GLOBAL_DEBUG_LEVEL      DBLD /* for high level driver files */

/************************************************************

	        DEBUG OUTPUT DEFINITION
   if undefined, the corresponding file has no debug output
************************************************************/

/* for highlevel driver files */
#define XTDC_DEBUG               /* debug of xtdc.c */


/************************************************************

	          SPECIFIC DEBUG LEVELS
 if defined, give the debug level specific for a given file
 otherwise, the global debug level of thedriver will be used
************************************************************/

/* for highlevel driver files */
/*
#define XTDC_DEBUG_LEVEL             DBLD
*/


/************************************************************

		DBG_* DEFINITIONS
		
   (should not be touched except when creating a new file)

************************************************************/

/************* for xtdc files *******************/

#ifdef XTDC_DEBUG
#  ifdef XTDC_DEBUG_LEVEL
#    define DBG_XTDC(l,f, args...) {if(l <= XTDC_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_XTDC(l,f, args...) {if(l <= XHIGHLEVEL_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_XTDC(l,f...) {}
#endif

#endif

