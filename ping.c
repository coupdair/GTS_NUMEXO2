#include "gts.h"


void processPingRequest (void *p)
{
int		n, nwrote, sd = (int)p, RECV_BUF_SIZE = 2048;
char	recv_buf [RECV_BUF_SIZE];

	while (1) {
// read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = read (sd, recv_buf, RECV_BUF_SIZE)) < 0) {
			perror ("ping.c::processPingRequest:read=>");
			break;
		}
// break if client closed connection or recv'd mesg is "quit"
		if ((! n) || (! strncmp (recv_buf, "quit", 4)))
			break;
// handle request
		if ((nwrote = write (sd, recv_buf, n)) < 0) {
			perror ("ping.c::processPingRequest:write=>");
			break;
		}
	}
	close (sd);
}


void pingServer ()
{
int					sock, new_sd;
struct sockaddr_in	address, remote;
socklen_t			size;

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("ping.c::pingServer:sock=>");
		return;
	}
	address.sin_family = AF_INET;
	address.sin_port = htons (ECHO_PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind (sock, (struct sockaddr *)&address, sizeof (address)) < 0) {
		perror ("ping.c::pingServer:bind=>");
		return;
	}
	listen (sock, 1);
	size = sizeof (remote);

	while (1) {
		if ((new_sd = accept (sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0)
			sys_thread_new ("ping_server", processPingRequest, 
				(void*)new_sd, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
	}
}
