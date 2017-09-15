#include "gts.h"

int gtsServSock;

extern int gtsCliSock;

void udpServer(void *);

extern int clientSetup(void);
extern void * gtsCmdInterpreter(void *);

void udpServer(void * param)
{
	int			taskId;
	int			reqSize, ackSize, one = 1;
	struct sockaddr_in	serverAddr, clientAddr;
	socklen_t		fromLen;
	GTScommands		*gtsCmds;
	GtsThreadArgs		gtsThreadArgs;
	pthread_t		tid;
	pthread_attr_t		attr;

	gtsCmds = gtsCommands;

	for (taskId=0; taskId < MaxGtsCommands; ++taskId, ++gtsCmds)
		gtsCmds->taskNameLen = strlen (gtsCmds->taskName);

//! - Setup gts Server INET
	memset((char *)&serverAddr, 0, sizeof serverAddr);
	memset((char *)&clientAddr, 0, sizeof clientAddr);

	if ((gtsServSock = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror ("udpServer.c::udpServer:socket=>");
		return;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons (TARGET_SERVER_PORT_NUM);

	fromLen = sizeof (struct sockaddr_in);
	reqSize = sizeof (UDP_request);
	ackSize = sizeof (UDP_ack);

	if (bind (gtsServSock, (struct sockaddr *)&serverAddr, sizeof serverAddr) < 0)  {
		perror ("udpServer.c::udpServer:bind=>");
		return;
	}

	setsockopt (gtsServSock, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));

//! - infinite loop (see following *)
	while (1) {
		memset (&gtsThreadArgs, 0, sizeof gtsThreadArgs);

		gtsThreadArgs.gtsCmd = gtsCmds;

		if (recvfrom (gtsServSock, (char *)&gtsThreadArgs.udpReq, reqSize, MSG_WAITALL,
					(struct sockaddr *)&clientAddr, &fromLen) < reqSize) {
			perror ("udpServer.c::udpServer:recvfrom=>");
			break;
		}
//		printf ("CMD=%s\n", gtsThreadArgs.udpReq.mesg); 

//! * setup client server if it failed
		if (gtsCliSock <= 0) gtsCliSock = clientSetup();

//! * launch thread
		pthread_attr_init (&attr);

		if (pthread_create (&tid, &attr, gtsCmdInterpreter, (void *)&gtsThreadArgs) != 0) {
			perror ("udpServer.c::udpServer:pthread_create=>");
			continue;
		}

//! * wait thread and process output
		switch (gtsThreadArgs.udpReq.type) {
			case UDP_NON_BLOCKING :
				pthread_join (tid, NULL);
				logAnswer();
				statusAnswer( gtsThreadArgs.udpAck );
				continue;

			case UDP_BLOCKING :
				pthread_join (tid, NULL);
				logAnswer();
				statusAnswer( gtsThreadArgs.udpAck );
				gtsThreadArgs.udpAck = ( gtsThreadArgs.udpAck == XST_SUCCESS ? ACK_SUCCESS : ACK_ERROR );
				break;
/*
			case UDP_TIMEOUT :
				usleep (2000);
				{

					p_stat	ps;
					process_status (tid, &ps);
					if (ps.state != PROC_DEAD) kill (tid);

				}
				logAnswer();
				break;
*/
			case UDP_RETURN_VALUE : case UDP_TIMEOUT :
				pthread_join (tid, NULL);
				logAnswer();
				break;
		}

//! * send output
		if (sendto (gtsServSock, (void *) &gtsThreadArgs.udpAck, ackSize, 0,
					(struct sockaddr *)&clientAddr, fromLen) < 0) {
			perror ("udpServer.c::udpServer:sendto=>");
			break;
		}
	}

//! - shutdown and close sockets
	shutdown (gtsServSock, 2);

	close (gtsServSock);

	shutdown (gtsCliSock, 2);

	close (gtsCliSock);
}
