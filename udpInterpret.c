#include "gts.h"

int	cmd_gtsTest (void*);
int	telnetResume (void*);
int	udpServer_Free (void*);
int	progHostTruncatedIpNumber (void*);
int	clientReset (void*);
int	cmd_gtsReset (void*);
int	cmd_gtsInit (void*);
int	cmd_gtsUpdate (void*);
int	cmd_transCon (void*);
int	cmd_transGtsTree (void*);
int	cmd_transDigitizer (void*);
int	cmd_conIs (void*);
int	cmd_gtsIs (void*);
int	cmd_treeSetup (void*);
int	cmd_treeStart (void*);
int	cmd_rxSystemIsReady (void*);
int	cmd_treeRead (void*);
int	cmd_treeStop (void*);
int	cmd_excludeCarrierForAlign (void*);
int	cmd_includeCarrierForAlign (void*);
int	cmd_alignStart (void*);
int	cmd_fineDelaySetNow (void*);
int	cmd_fineDelaySet (void*);
int	cmd_coarseDelaySet (void*);
int	cmd_alignSet (void*);
int	cmd_alignTdcSet (void*);
int	cmd_printBinary (void*);
int	cmd_set_output_file_name (void*);
int	cmd_alignMeas (void*);
int	cmd_tdcSet (void*);
int	cmd_tdcMeas (void*);
int	cmd_triggerSetup (void*);
int	cmd_triggerStart (void*);
int	cmd_excludeTriggerProcessor (void*);
int	cmd_testSet (void*);
int	cmd_loopbackSet (void*);
int	cmd_readField (void*);
int	cmd_readReg (void*);
int	cmd_readAll (void*);
int	cmd_writeField (void*);

typedef
	struct {
		int		truncatedIpNumber;
		int		mgtEnabled;
		int		inGtsTree;
		int		rxSystem;
	} GtsLeafInfo;

char	cmdName [128];


GTScommands	gtsCommands [MaxGtsCommands] = {
	{"%s",					"gtsTest",					cmd_gtsTest},
	{"%s",					"telnetResume",				telnetResume},
	{"%s",					"udpServer_Free",			udpServer_Free},
	{"%s %d",				"progTruncatedIpNumber",	progHostTruncatedIpNumber},
	{"%s",					"clientReset",				clientReset},
	{"%s",					"gtsReset",					cmd_gtsReset},
	{"%s %d %d %d %d",		"transCon",					cmd_transCon},
	{"%s %d %d %d %d",		"transGtsTree",				cmd_transGtsTree},
	{"%s %d %d %d %d",		"transDigitizer",			cmd_transDigitizer},
	{"%s",					"gtsUpdate",				cmd_gtsUpdate},
	{"%s",					"conIs",					cmd_conIs},
	{"%s",					"gtsIs",					cmd_gtsIs},
	{"%s %d",				"treeSetup",				cmd_treeSetup},
	{"%s",					"treeStart",				cmd_treeStart},
	{"%s %d",				"rxSystemIsReady",			cmd_rxSystemIsReady},
	{"%s %d",				"treeRead",					cmd_treeRead},
	{"%s",					"treeStop",					cmd_treeStop},
	{"%s",					"excludeCarrierForAlign",	cmd_excludeCarrierForAlign},
	{"%s",					"includeCarrierForAlign",	cmd_includeCarrierForAlign},
	{"%s",					"alignStart",				cmd_alignStart},
	{"%s %d",				"fineDelaySetNow",			cmd_fineDelaySetNow},
	{"%s %d",				"fineDelaySet",				cmd_fineDelaySet},
	{"%s %d",				"coarseDelaySet",			cmd_coarseDelaySet},
	{"%s %d %d",				"alignSet",					cmd_alignSet},
	{"%s %d %d %d %d",		"alignTdcSet",				cmd_alignTdcSet},
	{"%s %d",				"printBinary",				cmd_printBinary},
	{"%s %s",				"set_output_file_name",		cmd_set_output_file_name},
	{"%s %d %d",			"alignMeas",				cmd_alignMeas},
	{"%s %d %d %d",			"tdcSet",					cmd_tdcSet},
	{"%s %d %d %d",			"tdcMeas",					cmd_tdcMeas},
	{"%s %d %d",			"triggerSetup",				cmd_triggerSetup},
	{"%s %d",				"triggerStart",				cmd_triggerStart},
	{"%s",					"excludeTriggerProcessor",	cmd_excludeTriggerProcessor},
	{"%s",					"testSet",					cmd_testSet},
	{"%s",					"loopbackSet",					cmd_loopbackSet},
	{"%s %s %s",			"readField",				cmd_readField},
	{"%s %s",			"readReg",				cmd_readReg},
	{"%s",			"readAll",				cmd_readAll},
	{"%s %s %s %n",			"writeField",				cmd_writeField}
};


// Reduced version of scanf (%d, %x, %c, %n are supported)
// %d dec integer (E.g.: 12)
// %x hex integer (E.g.: 0xa0)
// %b bin integer (E.g.: b1010100010)
// %n hex, de or bin integer (e.g: 12, 0xa0, b1010100010)
// %c any character
//

int rsscanf (const char* str, const char* format, ...)
{
va_list		ap;
int			value, tmp;
int			count, pos;
char		neg, fmt_code, *s;
const char*	pf;

	va_start (ap, format);

	for (pf=format, count=0; *format != 0 && *str != 0; format++, str++) {
		while (*format == ' ' && *format != 0) format++;
		if (*format == 0) break;
		while (*str == ' ' && *str != 0) str++;
		if (*str == 0) break;
		if (*format == '%') {
			format++;
			if (*format == 'n') {
				if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
					fmt_code = 'x';
					str += 2;
				} else if (str[0] == 'b') {
					fmt_code = 'b';
					str++;
				} else
					fmt_code = 'd';
			} else
				fmt_code = *format;

			switch (fmt_code) {
				case 'x':
				case 'X':
					for (value = 0, pos = 0; *str != 0; str++, pos++) {
						if ('0' <= *str && *str <= '9')
							tmp = *str - '0';
						else if ('a' <= *str && *str <= 'f')
							tmp = *str - 'a' + 10;
						else if ('A' <= *str && *str <= 'F')
							tmp = *str - 'A' + 10;
						else
							break;
						value *= 16;
						value += tmp;
					}
					if (pos == 0)
						return count;
					*(va_arg (ap, int*)) = value;
					count++;
					break;

				case 'b':
					for (value = 0, pos = 0; *str != 0; str++, pos++) {
						if (*str != '0' && *str != '1') break;
						value *= 2;
						value += *str - '0';
					}
					if (pos == 0)
						return count;
					*(va_arg (ap, int*)) = value;
					count++;
					break;

				case 'd':
					if (*str == '-') {
						neg = 1;
						str++;
					} else
						neg = 0;
					for (value = 0, pos = 0; *str != 0; str++, pos++) {
						if ('0' <= *str && *str <= '9')
							value = value*10 + (int)(*str - '0');
						else
							break;
					}
					if (pos == 0)
						return count;
					*(va_arg (ap, int*)) = neg ? -value : value;
					count++;
					break;

				case 'c':
					*(va_arg (ap, char*)) = *str;
					count++;
					break;

				case 's':
					s = va_arg (ap, char*);
					while (*str != 0 && *str == ' ')
						str++;
					while (*str != 0) {
						if (*str == ' ' || *str == '\n') break;
						*s++ = *str++;
					}
					*s = 0;
					count++;
					break;

				default:
					return count;
			}
		} else {
			if (*format != *str)
				break;
		}
	}
	va_end (ap);
	return count;
}

int	cmd_gtsTest (void * param)
{
  return (int)gtsTest();
}

int	telnetResume (void * param)
{
	return 0;
}

int	udpServer_Free (void * param)
{
	return 0;
}

int	progHostTruncatedIpNumber (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs*)param;
  int ipNumber;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &ipNumber) != 2)
  {
    return 1;
  }

  return 0;
}

extern int gtsCliSock;

int	clientReset (void * param)
{
	shutdown (gtsCliSock, 2);

	close (gtsCliSock);

	gtsCliSock = clientSetup();

	return 0;
}

int	cmd_gtsReset (void * param)
{
	return gtsReset();
}

int	cmd_gtsInit (void * param)
{
	return gtsInit();
}

int	cmd_transCon (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs*)param;
  int con[4];
// As it is a leaf just use only the first paramter that is the top most SFP
// and forget about the remaining.
  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &con[0], &con[1], &con[2], &con[3]) != 5)
  {
    return 1;
  }

  return transCon( con[0], con[1], con[2], con[3] );
}

int	cmd_transGtsTree (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs*)param;
  int con[4];
// As it is a leaf just use only the first paramter that is the top most SFP
// and forget about the remaining.
  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &con[0], &con[1], &con[2], &con[3]) != 5)
  {
    return 1;
  }

  return transGtsTree( con[0], con[1], con[2], con[3] );
}

int	cmd_transDigitizer (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs*)param;
  int con[4];
// As it is a leaf just use only the first paramter that is the top most SFP
// and forget about the remaining.
  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &con[0], &con[1], &con[2], &con[3]) != 5)
  {
    return 1;
  }

  return transDigitizer( con[0], con[1], con[2], con[3] );
}

int	cmd_gtsUpdate (void * param)
{
	return gtsUpdate();
}

int	cmd_conIs (void * param)
{
	return conIs();
}

int	cmd_gtsIs (void * param)
{
	return gtsIs();
}

int	cmd_treeSetup (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int step;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &step) != 2)
  {
    return 1;
  }

  return treeSetup( step );
}

int	cmd_treeStart (void * param)
{
	return treeStart();
}

int	cmd_rxSystemIsReady (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int transceiver;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &transceiver) != 2)
  {
    return 1;
  }

  return rxSystemIsReady( transceiver );
}

int	cmd_treeRead (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int transceiver;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &transceiver) != 2)
  {
    return 1;
  }

  return (int)treeRead( transceiver );
}

int	cmd_treeStop (void * param)
{
	return treeStop();
}

int	cmd_excludeCarrierForAlign (void * param)
{
	return excludeCarrierForAlign();
}

int	cmd_includeCarrierForAlign (void * param)
{
	return includeCarrierForAlign();
}

int	cmd_alignStart (void * param)
{
	return alignStart();
}

int	cmd_fineDelaySetNow (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int delay;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &delay) != 2)
  {
    return 1;
  }

  return fineDelaySetNow( (unsigned int)delay );
}

int	cmd_fineDelaySet (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int delay;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &delay) != 2)
  {
    return 1;
  }

  return fineDelaySet( (unsigned int)delay );
}

int	cmd_coarseDelaySet (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int delay;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &delay) != 2)
  {
    return 1;
  }

  return coarseDelaySet( (unsigned int)delay );
}

int	cmd_alignSet (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int GTStype, forward;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &GTStype, &forward) != 3)
  {
    return 1;
  }

  return alignSet( GTStype, forward );
}

int	cmd_alignTdcSet (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int GTStype, forward, transceiver, newcal;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format,
              cmdName, &GTStype, &forward, &transceiver, &newcal) != 5)
  {
    return 1;
  }

  return alignTdcSet( GTStype, forward, transceiver, newcal );
}

int	cmd_printBinary (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int offset;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &offset) != 2)
  {
    return 1;
  }

  return printBinary( (unsigned int)offset );
}

int	cmd_set_output_file_name (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  char file_name[80];

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, file_name) != 2)
  {
    return 1;
  }

  return set_output_file_name( file_name );
}

int	cmd_alignMeas (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int forward, nmes;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &forward, &nmes) != 3)
  {
    return 1;
  }

  return alignMeas( forward, nmes );
}

int	cmd_tdcSet (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int meas, debug, delay;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &meas, &debug, &delay) != 4)
  {
    return 1;
  }

  return tdcSet( meas, debug, delay );
}

int	cmd_tdcMeas (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int meas, debug, nmes;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &meas, &debug, &nmes) != 4)
  {
    return 1;
  }

  return tdcMeas( meas, debug, nmes );
}

int	cmd_triggerSetup (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int GTStype;
  int step;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &GTStype, &step) != 3)
  {
    return 1;
  }

  return triggerSetup( GTStype, step );
}

int	cmd_triggerStart (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int GTStype;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, &GTStype) != 2)
  {
    return 1;
  }

  return triggerStart( GTStype );
}

int	cmd_excludeTriggerProcessor (void * param)
{
  return 0;
}

int	cmd_testSet (void * param)
{
  return leaveTestLoopback();
}

int	cmd_loopbackSet (void * param)
{
  return leaveLoopback();
}

int cmd_readField (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  char reg_name[80];
  char field_name[80];
  int status = XST_SUCCESS;
  int return_val = -1;

//  printf("cmd_readField: %s\n",gtsThreadArgs->udpReq.mesg);

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, reg_name, field_name) != 3)
  {
    return 1;
  }

  status |= XMMRegs_Reg_Associate(&XMMRegsDriver, XREG_UPDATE_ALL);
  if (status == XST_SUCCESS) return_val = XMMRegs_Reg_ReadField(&XMMRegsDriver, reg_name, field_name);

  return return_val;
}

int cmd_readReg (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  char reg_name[80];
  int status = XST_SUCCESS;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, reg_name) != 2)
  {
    return 1;
  }

  status |= XMMRegs_Reg_Associate(&XMMRegsDriver, XREG_UPDATE_ALL);
  if (status == XST_SUCCESS) status |= XMMRegs_Reg_ReadReg(&XMMRegsDriver, reg_name);

  return status;
}

int cmd_readAll (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  int status = XST_SUCCESS;

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName) != 1)
  {
    return 1;
  }

  status |= XMMRegs_Reg_Associate(&XMMRegsDriver, XREG_UPDATE_ALL);
  if (status == XST_SUCCESS) status |= XMMRegs_Reg_ReadAll(&XMMRegsDriver);

  return status;
}

int cmd_writeField (void * param)
{
  GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;
  char reg_name[80];
  char field_name[80];
  unsigned int val_field;
  int status = XST_SUCCESS;

  printf("cmd_writeField: %s\n",gtsThreadArgs->udpReq.mesg);

  if (rsscanf(gtsThreadArgs->udpReq.mesg, gtsThreadArgs->gtsCmd->format, cmdName, reg_name, field_name, &val_field) != 4)
  {
    return 1;
  }

  status |= XMMRegs_Reg_Associate(&XMMRegsDriver, XREG_UPDATE_ALL);

  if (status == XST_SUCCESS) status |= XMMRegs_Reg_WriteField(&XMMRegsDriver, reg_name, field_name, val_field);

  status |= XMMRegs_Reg_Write(&XMMRegsDriver);

  return status;
}

int isValidCommand (GtsThreadArgs * gtsThreadArgs)
{
	int taskId;
 
	gtsThreadArgs->gtsCmd = gtsCommands;

	for (taskId = 0; taskId < MaxGtsCommands; ++taskId, gtsThreadArgs->gtsCmd++) {

//		printf("isValidCommand: taskId=%d taskName=%s\n",taskId,gtsThreadArgs->gtsCmd->taskName);

		if (! strncmp (gtsThreadArgs->udpReq.mesg,
					gtsThreadArgs->gtsCmd->taskName,
					gtsThreadArgs->gtsCmd->taskNameLen))
			return taskId;
	}

	return -1;
}

void * gtsCmdInterpreter (void * param)
{
	int taskId;
	GtsThreadArgs * gtsThreadArgs = (GtsThreadArgs *)param;

	if ((taskId = isValidCommand (gtsThreadArgs)) < 0) {
		printf ("*** Invalid command %s ***\n", gtsThreadArgs->udpReq.mesg);
		pthread_exit (NULL);
	}

//	printf("%s\n",gtsThreadArgs->udpReq.mesg);

//	printf("taskId=%d, on appelle la fonction %s\n", taskId, gtsThreadArgs->gtsCmd->taskName);

	gtsThreadArgs->udpAck = gtsThreadArgs->gtsCmd->task (gtsThreadArgs);

	printf ("\t\t%s=>0x%x\n", gtsThreadArgs->gtsCmd->taskName, gtsThreadArgs->udpAck);

	pthread_exit (NULL);

	return NULL;
}
