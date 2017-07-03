//GTS_misc
/*-----------------------------------------------------------------------
File : gtsMisc.c
Description : NUMEXO2 GTS server (GANIL)
Authors : Frederic SAILLANT, 
-----------------------------------------------------------------------*/

#include "gts.h"

//! \bug [test] remove all code that is module related (temporary just for linkage with EPICS test)

unsigned int cardNumber = 225;

void setCardNumber(void)
{//IP address
  char ip[INET_ADDRSTRLEN];
  struct ifconf ifconf;
  struct ifreq ifreqs[2];
  int fd;

  memset( (void *)&ifconf, 0, sizeof(ifconf) );

  ifconf.ifc_buf = (char *)ifreqs;
  ifconf.ifc_len = sizeof(ifreqs);

  fd = socket( AF_INET, SOCK_STREAM, 0 );

  ioctl( fd, SIOCGIFCONF, (char *)&ifconf );

  close(fd);

  inet_ntop( AF_INET, &(((struct sockaddr_in *)&(ifreqs[1].ifr_addr))->sin_addr), ip, INET_ADDRSTRLEN );

  printf("\nIP address : %s\n", ip);

  cardNumber = ((struct sockaddr_in *)&(ifreqs[1].ifr_addr))->sin_addr.s_addr & 0xFF;
}//IP address

//! initialize globals
void initialize(void)
{
//! - memory map
  {//mmap

  int fd;

  fd = open("/dev/memory", O_RDWR);

  if (fd == -1)
  {
    perror("open(\"/dev/memory\")");

    return -1;
  }

  close(fd);

  }//mmap
}//initialize

