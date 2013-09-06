#include "gts.h"

void processTelnetRequest (void *p)
{
int		n, nwrote, sd = (int)p, RECV_BUF_SIZE = 2048;
char	recv_buf [RECV_BUF_SIZE];

	{
		char	*closeTelnet = "Sorry !!! Telnet server not ready\r\n";
		write (sd, closeTelnet, strlen (closeTelnet) + 1);
		close (sd);
		return;
	}

	while (1) {
// read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = read (sd, recv_buf, RECV_BUF_SIZE)) < 0) {
			perror ("telnetd.c::processTelentRequest:read=>");
			break;
		}
// break if client closed connection or recv'd mesg is "quit"
		if ((! n) || (! strncmp (recv_buf, "quit", 4)))
			break;

// handle request
		if ((nwrote = write (sd, recv_buf, n)) < 0) {
			perror ("telnetd.c::processTelentRequest:write=>");
			break;
		}
	}
	
// close connection
	close (sd);
}


void telnetServer ()
{
int					sock, new_sd;
struct sockaddr_in	address, remote;
socklen_t			size;
	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("telnetd.c::telentServer:sock=>");
		return;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons (TELNET_PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind (sock, (struct sockaddr *)&address, sizeof (address)) < 0) {
		perror ("telnetd.c::telentServer:bind=>");
		return;
	}

	listen (sock, 1);
	size = sizeof (remote);

	while (1) {
		if ((new_sd = accept (sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
			sys_thread_new ("telnetd", processTelnetRequest, 
				(void*)new_sd, THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
		}
	}
}
