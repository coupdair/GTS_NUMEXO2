#ifndef XMMREGS_I_H
#define XMMREGS_I_H

#include "xmmregs.h"

extern unsigned int cardNumber;

extern XMMRegs_Config XMMRegs_ConfigTable[];

extern XMMRegs XMMRegsDriver;

/* private API */

extern void print_binary(unsigned int *ba);
extern int  set_bit_to_0(int vec, int pos);
extern int  set_bit_to_1(int vec, int pos);
extern int  set_bit(int vec, int pos, int value);
extern int  microsleep(unsigned int microsec);
extern unsigned int  takeTime(void);
extern unsigned int  diffTime(unsigned int old_time_nsec, unsigned int new_time_nsec);

#endif
