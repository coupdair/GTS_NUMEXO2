#ifndef DEBUG_H
#define DEBUG_H

#include "xdebug_i.h"

/************************************************************

		user choices

************************************************************/

/* global default debug level */
#define GLOBAL_DEBUG_LEVEL DBLD

/* if undefined, the corresponding file has no debug output */
#define GTSINIT_DEBUG            /* file gtsInit.c */
#define ALIGN_DEBUG              /* align.c */
#define ALIGN_CLK_DEBUG          /* align_clk.c */
#define ALIGN_DELAY_DEBUG        /* align_delay.c */
#define ALIGN_SFP_DEBUG          /* align_sfp.c */
#define ALIGN_MGTDATA_DEBUG      /* align_mgtdata.c */
#define ALIGN_PATH_DEBUG         /* align_path.c */
#define ALIGN_TDC_DEBUG          /* align_tdc.c */
#define ALIGN_TREE_DEBUG         /* align_tree.c */
#define TRIGGER_DEBUG            /* trigger.c */
#define HANDLEIP_DEBUG           /* handleIP.c */
#define UDP_DEBUG                /* udp.c */
#define UDP_CLIENT_DEBUG         /* udp_client.c */
#define UDP_SERVER_DEBUG         /* udp_server.c */
#define UDP_INTERPRET_DEBUG      /* udp_interpret.c */

/* if defined, give the debug level specific for a given file */
/* 
#define GTSINIT_DEBUG_LEVEL            DBLD 
#define ALIGN_DEBUG_LEVEL              DBLD
#define ALIGN_CLK_DEBUG_LEVEL          DBLD
#define ALIGN_DELAY_DEBUG_LEVEL        DBLD
#define ALIGN_SFP_DEBUG_LEVEL          DBLD
#define ALIGN_MGTDATA_DEBUG_LEVEL      DBLD
#define ALIGN_PATH_DEBUG_LEVEL         DBLD
#define ALIGN_TDC_DEBUG_LEVEL          DBLD
#define ALIGN_TREE_DEBUG_LEVEL         DBLD
#define TRIGGER_DEBUG_LEVEL            DBLD
#define HANDLEIP_DEBUG_LEVEL           DBLD
#define UDP_DEBUG_LEVEL                DBLD
#define UDP_SERVER_DEBUG_LEVEL         DBLD
#define UDP_CLIENT_DEBUG_LEVEL         DBLD
#define UDP_INTERPRET_DEBUG_LEVEL      DBLD
*/


/************************************************************

		DBG_* DEFINITIONS
		
    (should not be touched except when using a new file)

************************************************************/


#ifdef GTSINIT_DEBUG
#  ifdef GTSINIT_DEBUG_LEVEL
#    define DBG_GTSINIT(l, f, args...) {if(l <= GTSINIT_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_GTSINIT(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_GTSINIT(l, args...) {}
#endif

#ifdef ALIGN_DEBUG
#  ifdef ALIGN_DEBUG_LEVEL
#    define DBG_ALIGN(l, f, args...) {if(l <= ALIGN_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN(l, args...) {}
#endif

#ifdef ALIGN_CLK_DEBUG
#  ifdef ALIGN_CLK_DEBUG_LEVEL
#    define DBG_ALIGN_CLK(l, f, args...) {if(l <= ALIGN_CLK_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_CLK(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_CLK(l, args...) {}
#endif

#ifdef ALIGN_CARRIER_DEBUG
#  ifdef ALIGN_CARRIER_DEBUG_LEVEL
#    define DBG_ALIGN_CARRIER(l, f, args...) {if(l <= ALIGN_CARRIER_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_CARRIER(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_CARRIER(l, args...) {}
#endif

#ifdef ALIGN_DELAY_DEBUG
#  ifdef ALIGN_DELAY_DEBUG_LEVEL
#    define DBG_ALIGN_DELAY(l, f, args...) {if(l <= ALIGN_DELAY_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_DELAY(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_DELAY(l, args...) {}
#endif

#ifdef ALIGN_SFP_DEBUG
#  ifdef ALIGN_SFP_DEBUG_LEVEL
#    define DBG_ALIGN_SFP(l, f, args...) {if(l <= ALIGN_SFP_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_SFP(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_SFP(l, args...) {}
#endif

#ifdef ALIGN_MGTDATA_DEBUG
#  ifdef ALIGN_MGTDATA_DEBUG_LEVEL
#    define DBG_ALIGN_MGTDATA(l, f, args...) {if(l <= ALIGN_MGTDATA_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_MGTDATA(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_MGTDATA(l, args...) {}
#endif

#ifdef ALIGN_PATH_DEBUG
#  ifdef ALIGN_PATH_DEBUG_LEVEL
#    define DBG_ALIGN_PATH(l, f, args...) {if(l <= ALIGN_PATH_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_PATH(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_PATH(l, args...) {}
#endif

#ifdef ALIGN_TDC_DEBUG
#  ifdef ALIGN_TDC_DEBUG_LEVEL
#    define DBG_ALIGN_TDC(l, f, args...) {if(l <= ALIGN_TDC_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_TDC(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_TDC(l, args...) {}
#endif

#ifdef ALIGN_TREE_DEBUG
#  ifdef ALIGN_TREE_DEBUG_LEVEL
#    define DBG_ALIGN_TREE(l, f, args...) {if(l <= ALIGN_TREE_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_ALIGN_TREE(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_ALIGN_TREE(l, args...) {}
#endif

#ifdef TRIGGER_DEBUG
#  ifdef TRIGGER_DEBUG_LEVEL
#    define DBG_TRIGGER(l, f, args...) {if(l <= TRIGGER_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_TRIGGER(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_TRIGGER(l, args...) {}
#endif

#ifdef HANDLEIP_DEBUG
#  ifdef HANDLEIP_DEBUG_LEVEL
#    define DBG_HANDLEIP(l, f, args...) {if(l <= HANDLEIP_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_HANDLEIP(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_HANDLEIP(l, args...) {}
#endif

#ifdef SYSNET_DEBUG
#  ifdef SYSNET_DEBUG_LEVEL
#    define DBG_SYSNET(l, f, args...) {if(l <= SYSNET_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_SYSNET(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_SYSNET(l, args...) {} 
#endif

#ifdef UDP_DEBUG
#  ifdef UDP_DEBUG_LEVEL
#    define DBG_UDP(l, f, args...) {if(l <= UDP_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_UDP(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_UDP(l, args...) {}
#endif

#ifdef UDP_SERVER_DEBUG
#  ifdef UDP_SERVER_DEBUG_LEVEL
#    define DBG_UDP_SERVER(l, f, args...) {if(l <= UDP_SERVER_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_UDP_SERVER(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_UDP_SERVER(l, args...) {}
#endif

#ifdef UDP_INTERPRET_DEBUG
#  ifdef UDP_INTERPRET_DEBUG_LEVEL
#    define DBG_UDP_INTERPRET(l, f, args...) {if(l <= UDP_INTERPRET_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_UDP_INTERPRET(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_UDP_INTERPRET(l, args...) {}
#endif

#ifdef UDP_CLIENT_DEBUG
#  ifdef UDP_CLIENT_DEBUG_LEVEL
#    define DBG_UDP_CLIENT(l, f, args...) {if(l <= UDP_CLIENT_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  else
#    define DBG_UDP_CLIENT(l, f, args...) {if(l <= GLOBAL_DEBUG_LEVEL) {eprintf(FIRST_ARG, f, ##args ); estrcpy();}}
#  endif
#else
#  define DBG_UDP_CLIENT(l, args...) {}
#endif

#endif
