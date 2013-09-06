#include "gts.h"

// for the moment we set the host IP address here ...
#define HOST_IP_ADDRESS "10.10.17.254"
//#define HOST_IP_ADDRESS "193.48.111.15"

int gtsCliSock = -1;

static struct sockaddr_in hostAddr;

int clientSetup(void);
int logAnswer(void);
int outputAnswer(void);
int statusAnswer(int);
static char itoa(int);

char itoa(int n)
{
  while ( (n/10) > 0 ) n /= 10;

  return ('0' + n);
}

int clientSetup(void)
{
  memset ((char *)&hostAddr, 0, sizeof hostAddr);

  if ((gtsCliSock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    perror ("udpClient.c::clientSetup:socket=>");
    return -1;
  }

  hostAddr.sin_family = AF_INET;
  hostAddr.sin_port = htons (HOST_SERVER_PORT_NUM);
  hostAddr.sin_addr.s_addr = inet_addr (HOST_IP_ADDRESS);

  return gtsCliSock;
}

int logAnswer(void)
{
  GtsThreadArgs gtsThreadArgs;

  gtsThreadArgs.udpAns.log.type = UDP_ANSWER_LOG;

  memset(gtsThreadArgs.udpAns.log.mesg,0,ANSWER_MSG_SIZE-1);

  strncpy(gtsThreadArgs.udpAns.log.mesg, global_log, ANSWER_MSG_SIZE-1);

  gtsThreadArgs.udpAns.log.mesg[ANSWER_MSG_SIZE-2] = 0;

  memset(global_log,0,sizeof(global_log));

//  printf("udpClient.c::logAnswer: gtsCliSock = %d\n",gtsCliSock);

  if (sendto(gtsCliSock,(void *)&gtsThreadArgs.udpAns.log,sizeof(UDP_answer_log),0,
             (struct sockaddr *)&hostAddr,sizeof(struct sockaddr_in)) < 0)
  {
    perror ("udpClient.c::logAnswer:sendto=>");
    shutdown(gtsCliSock, 2);
    close(gtsCliSock); 
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}

int outputAnswer(void)
{
  GtsThreadArgs gtsThreadArgs;

  memset(&gtsThreadArgs.udpAns.output.file_name,0,128);

  memset(&gtsThreadArgs.udpAns.output.mesg,0,ANSWER_MSG_SIZE-129);

  gtsThreadArgs.udpAns.output.type = UDP_ANSWER_OUTPUT;

  strncpy(gtsThreadArgs.udpAns.output.file_name, file_name_output, 127);

  gtsThreadArgs.udpAns.output.file_name[127] = 0;

  strncpy(gtsThreadArgs.udpAns.output.mesg, global_output, ANSWER_MSG_SIZE-129);

  gtsThreadArgs.udpAns.output.mesg[ANSWER_MSG_SIZE-130] = 0;

  memset(global_output,0,sizeof(global_output));

  if (sendto(gtsCliSock,(void *)&gtsThreadArgs.udpAns.output,sizeof(UDP_answer_output),0,
             (struct sockaddr *)&hostAddr,sizeof(struct sockaddr_in)) < 0)
  {
    perror ("udpClient.c::outputAnswer:sendto=>");
    shutdown(gtsCliSock, 2);
    close(gtsCliSock); 
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}

int statusAnswer(int status)
{
  GtsThreadArgs gtsThreadArgs;

  gtsThreadArgs.udpAns.status.status = itoa(status);

  memset(gtsThreadArgs.udpAns.status.unconnected,0,ANSWER_MSG_SIZE-2);

  gtsThreadArgs.udpAns.status.type = UDP_ANSWER_STATUS;

  if (sendto(gtsCliSock,(void *)&gtsThreadArgs.udpAns.status,sizeof(UDP_answer_status),0,
             (struct sockaddr *)&hostAddr,sizeof(struct sockaddr_in)) < 0)
  {
    perror ("udpClient.c::statusAnswer:sendto=>");
    shutdown(gtsCliSock, 2);
    close(gtsCliSock); 
    return XST_FAILURE; 
  }

  return XST_SUCCESS;
}
