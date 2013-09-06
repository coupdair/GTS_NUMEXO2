#ifndef ALIGN_H
#define ALIGN_H

#include "xspi_tdcgp2.h"

#define ROOT                 1
#define FANIN_FANOUT         2
#define LEAVE                3

#define FORCE_SET            1
#define DONT_FORCE_SET       0
#define BLOCKING             1
#define NON_BLOCKING         0

#define DO_NEW_CAL           1
#define KEEP_OLD_CAL         0

#define CARD_NUMBER_ERROR  254

#define MAX_RAW_DELAY_V3   1273 /* max delay in GTS_V3, expressed in units of DELAYLINE steps */
#define LIM_INF_DELAYLINE_LMKPLL 333 /* in units of DelayLine, corresponds to 3000ps */
#define LIM_SUP_DELAYLINE_LMKPLL 888 /* in units of DelayLine, corresponds to 8000ps */
#define FIXED_LMKPLL_DELAY 0xF /* fixed delay put in LmkPll delay block, in units of LmkPll delay*/
#define FIXED_LMKPLL_DELAY_IN_DELAYLINE_INCREMENT 250 /* FIXED_LMKPLL_DELAY express in units of DELAYLINE */

#define DELAY_IN_US_BETWEEN_LOG_IN_LOOP 100000 /* 100ms between log outputs */
                               /* be careful, you will never get a precision better than 200us */
                               /* given by sysClkRateSet(5000) in usrAppInit */

/* handles carrier included modes */

extern unsigned int alignModeRead(void);
extern int includeCarrierForAlign(void);
extern int excludeCarrierForAlign(void);

/* high level align APIs */

extern int alignSet(int GTStype, int forward);
extern int alignTdcSet(int GTStype, int forward, int transceiver, int newcal);
extern int alignMeas(int forward, int nmes);

/* align_clk */

extern int isPllLocked(void);
extern int rxSystemStatus(int transceiver);
extern int txSystemStatusAll(void);
extern int clkSet(int GTStype, int usrclk, int forward, int backward, int force_set, int block_mode);
extern int gtsReadySet(void);
extern int gtsReadyUnSet(void);

/* align_delay */

/* align_path */

extern int muxSyncSet(int GTStype, int forward, int transceiver);
extern int treeMuxPathSet(void);
extern int triggerMuxPathSet(int GTStype);

/* align_sfp */

extern int muxExtSet(int GTStype, int forward, int backward);

/* align_mgtdata */

extern int mgtDataCommaSet(void);
extern int mgtDataCommaUnSet(void);

/* align_tdc */

extern int tdcSet(int meas, int debug, int delay);
extern int tdcStop(void);
extern int tdcMeas(int meas, int debug, int nmes);

#endif
