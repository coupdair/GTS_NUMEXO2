#include "xmmregs_i.h"
#include "xdebug_l.h"
#include <time.h>
#include <math.h>
#include <stdlib.h>

/* library type functions used by all components of the driver. 
These functions are declared in xmmregs_i.h */

#define DBG XMMR_I_DBG

void print_binary(unsigned int *ba)
{
  int i;

  for (i = 0; i < 32; i++)
  {
    if ( ( !(i%4) ) && i ) DBG(DBLI, " : ");

    DBG(DBLI, "%d", (int)( ( (*ba) << i ) >> 31 ) );
  }

  DBG(DBLI, "\n");
}

int set_bit_to_0(int vec, int pos)
{
  int mask = 0;
  
  mask = ~(1 << pos);
  return (vec & mask);
}

int set_bit_to_1(int vec, int pos)
{
  int mask = 0;
  
  mask = (1 << pos);
  return (vec | mask);
}

int set_bit(int vec, int pos, int value)
{
  if ( (value != 0) && (value != 1) ) {
    DBG(DBLE, "ERROR : out of range on value in function set_bit\n");
    return vec; 
  }
  
  if (value)
    return set_bit_to_1(vec,pos);
  else
    return set_bit_to_0(vec,pos);
}

int microsleep(unsigned int microsec)
{
  int status = XST_SUCCESS;
  const int onesec_in_microsec = 1000000;
  unsigned int sec = 0, nanosec = 0;
  struct timespec requested = {0, 0}; /* nb of seconds, nb of nanoseconds*/
  
  sec = floor( ( (double) microsec ) / ((double)onesec_in_microsec) );
  nanosec = ( microsec % onesec_in_microsec ) * 1000;
  requested.tv_sec = sec;
  requested.tv_nsec = nanosec;
  status |= nanosleep(&requested, NULL);

  return status;
}

unsigned int takeTime(void)
{
  struct timespec current_time;

  if (clock_gettime(CLOCK_REALTIME, &current_time) != XST_SUCCESS) {
    DBG(DBLE, "ERROR : clock_gettime can't return current time\n");
    return -1; /* all Fs */
  }
  return current_time.tv_nsec;
}

unsigned int takeRes(void)
{
  struct timespec current_res;

  if (clock_getres(CLOCK_REALTIME, &current_res) != XST_SUCCESS) {
    DBG(DBLE, "ERROR : clock_getres can't return current res\n");
    return -1; /* all Fs */
  }
  DBG(DBLI, "%lu sec : %lu nsec : %lu usec\n", current_res.tv_sec, current_res.tv_nsec, current_res.tv_nsec/1000 );

  return current_res.tv_nsec;
}

/* arguments are in nanoseconds and can't be bigger than 1 sec */
unsigned int diffTime(unsigned int old_time_nsec, unsigned int new_time_nsec)
{
  static unsigned int one_sec_in_nsec = 1000000000;
  if (new_time_nsec >= old_time_nsec)
    return (new_time_nsec - old_time_nsec);
  else
    return (one_sec_in_nsec + old_time_nsec - new_time_nsec);
}

#undef DBG
