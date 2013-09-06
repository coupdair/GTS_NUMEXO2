#ifndef __UDPSTRUCT_H_
#define __UDPSTRUCT_H_
//////////////////////////////////
// GTS Communication Parameters //
//////////////////////////////////
#define TARGET_SERVER_PORT_NUM  5002    // target server's port for bind()
#define HOST_SERVER_PORT_NUM    5254    // host server's port for bind()
#define REQUEST_MSG_SIZE        1024    // max size of request message 
#define ANSWER_MSG_SIZE         1024    // max size of answer message
 
// the following constants define the type of 'ack' given to the host
#define ERROR_32                0xFFFFFFFF
#define ACK_SUCCESS             '0'
#define ACK_ERROR               '1'

// the following constants define the type of 'req' given by the host
#define UDP_NON_BLOCKING        '1'
#define UDP_BLOCKING            '2'
#define UDP_RETURN_VALUE        '3' 
#define UDP_TIMEOUT             '4'  
 
// the following constants define the type of 'answer' given to the host
#define UDP_ANSWER_STATUS       '1'
#define UDP_ANSWER_LOG          '2'
#define UDP_ANSWER_OUTPUT       '3'


typedef	unsigned UDP_ack;

typedef
    struct {
// request type : blocking,non-blocking,returned_value,timeout
		char	type;
// message buffer
		char	mesg [REQUEST_MSG_SIZE-1];
    } UDP_request;


typedef
    struct {
		char	mesg [ANSWER_MSG_SIZE]; 
    } UDP_answer_base;


typedef
    struct {
		char	type;		// type of answer
		char	status;		// return status of the function
		char	unconnected [ANSWER_MSG_SIZE-2];
    } UDP_answer_status;


typedef
    struct {
		char	type;
		char	mesg [ANSWER_MSG_SIZE-1]; 
    } UDP_answer_log;


typedef
    struct {
		char	type;
		char	file_name [128];
		char	mesg [ANSWER_MSG_SIZE-128-1]; 
    } UDP_answer_output;


typedef
    union {
		UDP_answer_base		base;
		UDP_answer_status	status;
		UDP_answer_log		log;
		UDP_answer_output	output;
    } UDP_answer;

typedef
    struct {
		char	*format;
		char	*taskName;
		int		(*task)(void*);
		int		taskNameLen;
	} GTScommands;

typedef
	struct {
		UDP_request		udpReq;
		UDP_ack			udpAck;
		UDP_answer		udpAns;
		GTScommands		*gtsCmd;
	} GtsThreadArgs;

#endif
