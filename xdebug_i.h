#ifndef XDEBUG_I_H
#define XDEBUG_I_H

#include <stdio.h>
#include "xstatus.h"

#define DBLE 1 /* error */
#define DBLW 2 /* warning */
#define DBLI 3 /* info */
#define DBLD 4 /* debug */

#define DEBUG_THROUGH_TERMINAL 0
#define DEBUG_THROUGH_ETHERNET 1
#define MAX_CHAR_LOG 1016
#define MAX_CHAR_SERIAL 1016

extern void estrcpy(void);
extern void s_strcpy_log(void);
extern void s_strcpy(void);
extern int dbgTermSet(void);

extern int logAnswer(void);

extern char last_sentence_serial[MAX_CHAR_SERIAL];
extern char last_sentence_log[MAX_CHAR_LOG];
extern char global_log[MAX_CHAR_LOG];

extern int  choice_terminal_ethernet;
extern char *FIRST_ARG;

#define eprintf sprintf

/************************************************************

		user choices

************************************************************/

/************************************************************

	          GLOBAL DEBUG LEVELS

    define the global debug levels for each set of drivers

************************************************************/

#define XDEBUG_GLOBAL_LEVEL           DBLD /* for xdebug files */

/************************************************************

	        DEBUG OUTPUT DEFINITION
   if undefined, the corresponding file has no debug output
************************************************************/

/* for xdebug driver files */
//#define XDEBUG_I_DEBUG             /* debug of xdebug_i.c */

/************************************************************

	          SPECIFIC DEBUG LEVELS
 if defined, give the debug level specific for a given file
 otherwise, the global debug level of thedriver will be used
************************************************************/

/* #define XDEBUG_I_DEBUG_LEVEL    DBLI */ 

/************************************************************

		DBG_* DEFINITIONS
		
   (should not be touched except when creating a new file)

************************************************************/

#ifdef XDEBUG_I_DEBUG
#  ifdef XDEBUG_I_DEBUG_LEVEL
#    define XDEBUG_I_DBG(l,f, args...) {if(l <= XDEBUG_I_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XDEBUG_I_DBG(l,f, args...) {if(l <= XDEBUG_GLOBAL_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XDEBUG_I_DBG(l,f...) {}
#endif


#endif

