#ifndef __GTS_H_
#define __GTS_H_

#include <unistd.h>
/*
#include "xmk.h"
#include "xtmrctr.h"
#include "xuartns550_l.h"
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>

#include "udpStruct.h"
#include "gtsCommands.h"
#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xdebug_i.h"
#include "xoutput_i.h"
#include "xspi.h"
#include "gtsMisc.h"

// General Definitions
//#define printf					xil_printf
#define	clrscr()				printf ("%c[2J\r\n", 27)
#define	gotoxy(x, y)			printf ("%c[%d;%dH", 27, y, x)
#define	delay(ms)				sys_sleep (ms)
#define	sleep(sec)				delay (sec*1000)

// OS specific definitions
#define	THREAD_STACKSIZE		4096

// GTS specific definitions
#define	TELNET_PORT				23
#define	ECHO_PORT				 7

#endif	// __GTS_H_
