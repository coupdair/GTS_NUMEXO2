#ifndef XDEBUG_L_H
#define XDEBUG_L_H
#include "xdebug_i.h"

/************************************************************

		user choices

************************************************************/

/************************************************************

	          GLOBAL DEBUG LEVELS

    define the global debug levels for each set of drivers

************************************************************/

#define XMMR_GLOBAL_DEBUG_LEVEL      DBLD /* for xmmregs driver files */
#define XSPI_GLOBAL_DEBUG_LEVEL      DBLD /* for xspi driver files */
#define XEMC_GLOBAL_DEBUG_LEVEL      DBLD /* for xemc driver files */

/************************************************************

	        DEBUG OUTPUT DEFINITION
   if undefined, the corresponding file has no debug output
************************************************************/

/* for xmmregs driver files */
#define XMMR_DEBUG             /* debug of xmmregs.c */
#define XMMR_I_DEBUG           /* debug of xmmregs_i.c */
#define XMMR_DATAPATH_DEBUG    /* debug of xmmregs_DataPath.c */
#define XMMR_CLOCKPATH_DEBUG   /* debug of xmmregs_ClockPath.c */
#define XMMR_ROCKETIO_DEBUG    /* debug of xmmregs_RocketIO.c */
#define XMMR_MUXINOUT_DEBUG    /* debug of xmmregs_MuxInout.c */
#define XMMR_TDCDEBUG_DEBUG    /* debug of xmmregs_TdcDebug.c */
#define XMMR_TRIGGER_DEBUG     /* debug of xmmregs_Trigger.c */
#define XMMR_LMKPLL_DEBUG      /* debug of xmmregs_LmkPll.c */
#define XMMR_REG_DEBUG         /* debug of xmmregs_Reg.c */

/* for xspi driver files */
#define XSPI_TDCGP2_DEBUG      /* debug of xspi_tdcgp2.c */

/* for xemc driver files */
#define XEMC_FLASH_DEBUG      /* debug of xemc_flash.c */

/************************************************************

	          SPECIFIC DEBUG LEVELS
 if defined, give the debug level specific for a given file
 otherwise, the global debug level of the driver will be used
************************************************************/

/* for xmmregs driver files */

//#define XMMR_DEBUG_LEVEL             DBLD
//#define XMMR_I_DEBUG_LEVEL           DBLD
//#define XMMR_DATAPATH_DEBUG_LEVEL    DBLD
//#define XMMR_CLOCKPATH_DEBUG_LEVEL   DBLD
//#define XMMR_ROCKETIO_DEBUG_LEVEL    DBLD 

//#define XMMR_MUXINOUT_DEBUG_LEVEL    DBLD 
//#define XMMR_TDCDEBUG_DEBUG_LEVEL    DBLD 
//#define XMMR_TRIGGER_DEBUG_LEVEL     DBLD 
//#define XMMR_LMKPLL_DEBUG_LEVEL      DBLD 
//#define XMMR_REG_DEBUG_LEVEL         DBLD 


/* for xspi driver files */
#define XSPI_TDCGP2_DEBUG_LEVEL      DBLI

/* for xemc driver files */
/* #define XEMC_FLASH_DEBUG_LEVEL    DBLD */





/************************************************************

		DBG_* DEFINITIONS
		
   (should not be touched except when creating a new file)

************************************************************/

/************* for xmmregs driver files *******************/

#ifdef XMMR_DEBUG
#  ifdef XMMR_DEBUG_LEVEL
#    define XMMR_DBG(l,f, args...) {if(l <= XMMR_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_DBG(l,f...) {}
#endif

#ifdef XMMR_I_DEBUG
#  ifdef XMMR_I_DEBUG_LEVEL
#    define XMMR_I_DBG(l,f, args...) {if(l <= XMMR_I_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_I_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_I_DBG(l,f...) {}
#endif

#ifdef XMMR_DATAPATH_DEBUG
#  ifdef XMMR_DATAPATH_DEBUG_LEVEL
#    define XMMR_DATAPATH_DBG(l,f, args...) {if(l <= XMMR_DATAPATH_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_DATAPATH_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_DATAPATH_DBG(l,f...) {}
#endif

#ifdef XMMR_CLOCKPATH_DEBUG
#  ifdef XMMR_CLOCKPATH_DEBUG_LEVEL
#    define XMMR_CLOCKPATH_DBG(l,f, args...) {if(l <= XMMR_CLOCKPATH_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_CLOCKPATH_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_CLOCKPATH_DBG(l,f...) {}
#endif

#ifdef XMMR_ROCKETIO_DEBUG
#  ifdef XMMR_ROCKETIO_DEBUG_LEVEL
#    define XMMR_ROCKETIO_DBG(l,f, args...) {if(l <= XMMR_ROCKETIO_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_ROCKETIO_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_ROCKETIO_DBG(l,f...) {}
#endif

#ifdef XMMR_MUXINOUT_DEBUG
#  ifdef XMMR_MUXINOUT_DEBUG_LEVEL
#    define XMMR_MUXINOUT_DBG(l,f, args...) {if(l <= XMMR_MUXINOUT_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_MUXINOUT_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_MUXINOUT_DBG(l,f...) {}
#endif

#ifdef XMMR_TDCDEBUG_DEBUG
#  ifdef XMMR_TDCDEBUG_DEBUG_LEVEL
#    define XMMR_TDCDEBUG_DBG(l,f, args...) {if(l <= XMMR_TDCDEBUG_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_TDCDEBUG_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_TDCDEBUG_DBG(l,f...) {}
#endif

#ifdef XMMR_TRIGGER_DEBUG
#  ifdef XMMR_TRIGGER_DEBUG_LEVEL
#    define XMMR_TRIGGER_DBG(l,f, args...) {if(l <= XMMR_TRIGGER_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_TRIGGER_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_TRIGGER_DBG(l,f...) {}
#endif

#ifdef XMMR_LMKPLL_DEBUG
#  ifdef XMMR_LMKPLL_DEBUG_LEVEL
#    define XMMR_LMKPLL_DBG(l,f, args...) {if(l <= XMMR_LMKPLL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_LMKPLL_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_LMKPLL_DBG(l,f...) {}
#endif

#ifdef XMMR_REG_DEBUG
#  ifdef XMMR_REG_DEBUG_LEVEL
#    define XMMR_REG_DBG(l,f, args...) {if(l <= XMMR_REG_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XMMR_REG_DBG(l,f, args...) {if(l <= XMMR_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XMMR_REG_DBG(l,f...) {}
#endif

/************* for xspi driver files *******************/

#ifdef XSPI_TDCGP2_DEBUG
#  ifdef XSPI_TDCGP2_DEBUG_LEVEL
#    define XSPI_TDCGP2_DBG(l,f, args...) {if(l <= XSPI_TDCGP2_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XSPI_TDCGP2_DBG(l,f, args...) {if(l <= XSPI_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XSPI_TDCGP2_DBG(l,f...) {}
#endif

/************* for xemc driver files *******************/

#ifdef XEMC_FLASH_DEBUG
#  ifdef XEMC_FLASH_DEBUG_LEVEL
#    define XEMC_FLASH_DBG(l,f, args...) {if(l <= XEMC_FLASH_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define XEMC_FLASH_DBG(l,f, args...) {if(l <= XEMC_GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define XEMC_FLASH_DBG(l,args...) {}
#endif

#endif

