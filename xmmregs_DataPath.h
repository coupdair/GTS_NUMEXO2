#ifndef XMMREGS_DATAPATH_H
#define XMMREGS_DATAPATH_H

#define SY_SYNC_USE    1
#define SY_SYNC_BYPASS 0

#define SY_SFP         1
#define SY_MGT_MASTER  2
#define SY_SYNC        3
#define SY_SFP_MASTER  4

/* transforms the values of the previous constants to : 
#define SY_SFP         1
#define SY_MGT_MASTER  0
#define SY_SYNC        1
#define SY_SFP_MASTER  0 
I have done this to avoid interpretation problems for the constants SY_*
Note that if x is out of range, it returns 0
*/
#define MUX_SYNC(x) ( (x == SY_SFP) || (x == SY_SYNC) ) ? 1 : 0 


#endif
