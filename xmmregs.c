#include "xmmregs.h"
#include "xmmregs_i.h"
#include "xdebug_l.h"

#define DBG XMMR_DBG

/************************************************************

		LOW-LEVEL STATIC FUNCTIONS

************************************************************/

static XMMRegs_StatusTransceiver connect_transceiver(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int index = 0;

  index = ( 1 << transceiver ); 
  Status.connected |= index;

  return Status;
}

static XMMRegs_StatusTransceiver disconnect_transceiver(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int index = 0;

  index = ~( 1 << transceiver ); 
  Status.connected &= index;

  return Status;
}

static int is_transceiver_connected(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int value;

  value = ( (Status.connected) & (1 << transceiver) ); 
  value = (value >> transceiver);

  return (int)value;
}

static XMMRegs_StatusTransceiver connect_transceiver_to_digitizer(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int index = 0;

  index = ( 1 << transceiver ); 
  Status.digitizer |= index;

  return Status;
}

static XMMRegs_StatusTransceiver disconnect_transceiver_from_digitizer(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int index = 0;

  index = ~( 1 << transceiver ); 
  Status.digitizer &= index;

  return Status;
}

static int is_transceiver_connected_to_digitizer(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int value;

  value = ( (Status.digitizer) & (1 << transceiver) ); 
  value = (value >> transceiver);

  return (int)value;
}

static XMMRegs_StatusTransceiver insert_transceiver_in_gts_tree(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int index = 0;

  index = ( 1 << transceiver ); 
  Status.gts_tree |= index;

  return Status;
}

static XMMRegs_StatusTransceiver remove_transceiver_from_gts_tree(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int index = 0;

  index = ~( 1 << transceiver ); 
  Status.gts_tree &= index;

  return Status;
}

static int is_transceiver_in_gts_tree(XMMRegs_StatusTransceiver Status, int transceiver)
{
  unsigned int value;

  value = ( (Status.gts_tree) & (1 << transceiver) ); 
  value = (value >> transceiver);

  return (int)value;
}

/************************************************************

		GENERAL FUNCTIONS

************************************************************/

void XMMRegs_PrintBinary(XMMRegs *InstancePtr, unsigned int offset)
{
  unsigned int *ba = (unsigned int *)(InstancePtr->BaseAddress + offset);

  print_binary(ba);

  return;
}

void XMMRegs_PrintHex(XMMRegs *InstancePtr, unsigned int offset)
{
  unsigned int *ba = (unsigned int *)(InstancePtr->BaseAddress + offset);
  unsigned int val = *ba;

  DBG(DBLI, "0x%08X\n", val);

  return;
}

unsigned int XMMRegs_ReadRegister(XMMRegs *InstancePtr, unsigned int offset)
{
  unsigned int *ba = (unsigned int *)(InstancePtr->BaseAddress + offset);
  unsigned int val = *ba;

  return val;
}

int XMMRegs_Transceiver_Connect(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_Transceiver_Connect\n");
    status = XST_FAILURE;
  }
  else {
    InstancePtr->Status = connect_transceiver(InstancePtr->Status, transceiver);
  }

  return status;
}

int XMMRegs_Transceiver_DisConnect(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_Transceiver_DisConnect\n");
    status = XST_FAILURE;
  }
  else {
    InstancePtr->Status = disconnect_transceiver(InstancePtr->Status, transceiver);
  }

  return status;
}

int XMMRegs_IsTransceiverConnected(XMMRegs *InstancePtr, int transceiver)
{
  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_IsTransceiverConnected\n");
    return TRANSCEIVER_IS_NOT_CONNECTED;
  }
  else {
    return is_transceiver_connected(InstancePtr->Status, transceiver);
  }
}

int  XMMRegs_TransceiverGtsTree_Insert(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_TransceiverGtsTree_Insert\n");
    status = XST_FAILURE;
  }
  else {
    InstancePtr->Status = insert_transceiver_in_gts_tree(InstancePtr->Status, transceiver);
  }

  return status;
}

int  XMMRegs_TransceiverGtsTree_Remove(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_TransceiverGtsTree_Remove\n");
    status = XST_FAILURE;
  }
  else {
    InstancePtr->Status = remove_transceiver_from_gts_tree(InstancePtr->Status, transceiver);
  }

  return status;
}

int XMMRegs_IsTransceiverInGtsTree(XMMRegs *InstancePtr, int transceiver)
{
  int status = TRANSCEIVER_IS_IN_GTS_TREE;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_IsTransceiverInGtsTree\n");
    return TRANSCEIVER_IS_NOT_IN_GTS_TREE;
  }
  else {
    status &= is_transceiver_connected(InstancePtr->Status, transceiver);
    status &= is_transceiver_in_gts_tree(InstancePtr->Status, transceiver);
  }

  return status;
}

int XMMRegs_TransceiverDigitizer_Connect(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_TransceiverDigitizer_Connect\n");
    status = XST_FAILURE;
  }
  else {
    InstancePtr->Status = connect_transceiver_to_digitizer(InstancePtr->Status, transceiver);
  }

  return status;
}

int XMMRegs_TransceiverDigitizer_DisConnect(XMMRegs *InstancePtr, int transceiver)
{
  int status = XST_SUCCESS;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_TransceiverDigitizer_DisConnect\n");
    status = XST_FAILURE;
  }
  else {
    InstancePtr->Status = disconnect_transceiver_from_digitizer(InstancePtr->Status, transceiver);
  }

  return status;
}

int XMMRegs_IsTransceiverConnectedToDigitizer(XMMRegs *InstancePtr, int transceiver)
{
  int status = TRANSCEIVER_IS_CONNECTED_TO_DIGITIZER;

  if ( (transceiver < TRANSCEIVER_0) || (transceiver > TRANSCEIVER_3) ) {
    DBG(DBLE, "ERROR : out of range error in function XMMRegs_IsTransceiverConnected\n");
    return TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER;
  }
  else {
    status &= is_transceiver_connected(InstancePtr->Status, transceiver);
    status &= is_transceiver_connected_to_digitizer(InstancePtr->Status, transceiver);
  }

  return status;
}

/************************************************************

		STATUS FUNCTIONS

************************************************************/

int XMMRegs_Status_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Transceiver_Connect(InstancePtr, TRANSCEIVER_0);
  status |= XMMRegs_TransceiverGtsTree_Insert(InstancePtr, TRANSCEIVER_0);
  status |= XMMRegs_TransceiverDigitizer_DisConnect(InstancePtr, TRANSCEIVER_0);

  return status;
}

int XMMRegs_Status_Start(XMMRegs *InstancePtr)
{
  return XST_SUCCESS;
}

int XMMRegs_Status_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Transceiver_DisConnect(InstancePtr, TRANSCEIVER_0);
  status |= XMMRegs_TransceiverGtsTree_Remove(InstancePtr, TRANSCEIVER_0);
  status |= XMMRegs_TransceiverDigitizer_DisConnect(InstancePtr, TRANSCEIVER_0);

  return status;
}

int XMMRegs_Status_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Status_Setup(InstancePtr);
  status |= XMMRegs_Status_Start(InstancePtr);

  return status;
}

int XMMRegs_Status_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Status_Stop(InstancePtr);
  status |= XMMRegs_Status_Init(InstancePtr);

  return status;
}

void XMMRegs_Status_PrintAll(XMMRegs *InstancePtr)
{
  XMMRegs_StatusTransceiver s = InstancePtr->Status;

  DBG(DBLI, "\nPrintAll of Status :--------------------------------------------------------------\n");
  DBG(DBLI, "\t\t\t:\t  28 :   24 :   20 :   16 :   12 :    8 :    4 :    0\n");
  DBG(DBLI, "StatusTransceiver\t:\t"); 
  print_binary((unsigned int *)&s);
}

int XMMRegs_Status_Check(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;
  int trans, dig, gts;

  /* MASTER_TRANSCEIVER = TRANSCEIVER_0 HAS TO BE IN GTS TREE */
  if ( XMMRegs_IsTransceiverConnected(InstancePtr, MASTER_TRANSCEIVER) == TRANSCEIVER_IS_CONNECTED ) 
  {
    dig = XMMRegs_IsTransceiverConnectedToDigitizer(InstancePtr, MASTER_TRANSCEIVER);
    gts = XMMRegs_IsTransceiverInGtsTree(InstancePtr, MASTER_TRANSCEIVER);

    if ( (gts == TRANSCEIVER_IS_IN_GTS_TREE) && (dig == TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER) )
    {
      /* OK */ status |= XST_SUCCESS;
    }
    else if (dig == TRANSCEIVER_IS_CONNECTED_TO_DIGITIZER)
    {
      DBG(DBLE, "ERROR : the master transceiver can't be connected to the digitizer\n");
      status |= XST_FAILURE; 
    }
    else if (gts == TRANSCEIVER_IS_NOT_IN_GTS_TREE)
    {
      DBG(DBLE, "ERROR : the master transceiver has to be connected to the gts tree\n");
      status |= XST_FAILURE; 
    }
    else 
    { /* unrecognized case */
      DBG(DBLE, "ERROR : in function XMMRegs_Status_Check\n");
      status |= XST_FAILURE; 
    }
  }
  else
  {
    DBG(DBLE, "ERROR : the master transceiver has to be connected\n"); 
    status |= XST_FAILURE;
  }

  return status;
}
    
/************************************************************

		SETTING FUNCTIONS

************************************************************/
  
int XMMRegs_FineDelayParameter_Set(XMMRegs *InstancePtr, unsigned int step_size, unsigned int step_interval)
{
  if (step_size >= MAX_DELAY)
  {
    DBG(DBLE, "ERROR : step_size should be smaller than %d in XMMRegs_FineDelayParameter_Set\n", MAX_DELAY);
    return XST_FAILURE; 
  }

  InstancePtr->Settings.fineDelayStepSize = step_size;
  InstancePtr->Settings.fineDelayStepInterval = step_interval; /* in us */

  return XST_SUCCESS;
}

int XMMRegs_LmkPllDelayParameter_Set(XMMRegs *InstancePtr, unsigned int step_size, unsigned int step_interval)
{
  if (step_size >= MAX_LMKPLL_DELAY)
  {
    DBG(DBLE, "ERROR : step_size should be smaller than %d in XMMRegs_LmkPllDelayParameter_Set\n", MAX_LMKPLL_DELAY);
    return XST_FAILURE; 
  }

  InstancePtr->Settings.LmkPllDelayStepSize = step_size;
  InstancePtr->Settings.LmkPllDelayStepInterval = step_interval; /* in us */

  return XST_SUCCESS;
}

unsigned int XMMRegs_AlignMode_Read(XMMRegs *InstancePtr)
{
  unsigned int align_mode;

  align_mode = InstancePtr->Settings.alignMode;

  if ( (align_mode != CARRIER_INCLUDED) && (align_mode != CARRIER_EXCLUDED) ) {
    DBG(DBLE, "ERROR : overflow on align_mode in XMMRegs_AlignMode_Read\n");  
  }

  return align_mode;
}

int XMMRegs_AlignMode_Set(XMMRegs *InstancePtr, unsigned int align_mode)
{
  if ( (align_mode != CARRIER_INCLUDED) && (align_mode != CARRIER_EXCLUDED) ) {
    DBG(DBLE, "ERROR : overflow on align_mode in XMMRegs_AlignMode_Set\n");  
    return XST_FAILURE;
  }

  InstancePtr->Settings.alignMode = align_mode;

  return XST_SUCCESS;
}
  
int XMMRegs_Settings_Setup(XMMRegs *InstancePtr)    
{
  int status = XST_SUCCESS;

  status |= XMMRegs_FineDelayParameter_Set(InstancePtr, DELAYLINE_STEP_SIZE_DEFAULT, DELAYLINE_STEP_INTERVAL_DEFAULT);
  status |= XMMRegs_LmkPllDelayParameter_Set(InstancePtr, LMKPLL_STEP_SIZE_DEFAULT, LMKPLL_STEP_INTERVAL_DEFAULT);

  if ( XMMRegs_Trigger_CarrierType_Read(InstancePtr) == LLP_CARRIER_IS_CARRIER ) {
    DBG(DBLI, "LLP Carrier detected => LLP_Carrier is included for the alignment\n");
    status |= XMMRegs_AlignMode_Set(InstancePtr, CARRIER_INCLUDED);
  }
  else {
    DBG(DBLI, "VME Carrier detected => Carrier is excluded for the alignment\n");
    status |= XMMRegs_AlignMode_Set(InstancePtr, CARRIER_EXCLUDED);
  }

  InstancePtr->Identification.gts_version         = GTS_VERSION;
  InstancePtr->Identification.rev_project         = REV_PROJECT;
  InstancePtr->Identification.rev_specific_driver = REV_SPECIFIC_DRIVER;
  InstancePtr->Identification.rev_shared_driver   = REV_SHARED_DRIVER;
  InstancePtr->Identification.carrier_type        = XMMRegs_Trigger_CarrierType_Read(InstancePtr);
  InstancePtr->Identification.trigger_type        = XMMRegs_Trigger_CoreType_Get(InstancePtr);

  return status;
}

int XMMRegs_Settings_Start(XMMRegs *InstancePtr)    
{
  return XST_SUCCESS;
}

int XMMRegs_Settings_Stop(XMMRegs *InstancePtr)    
{
  int status = XST_SUCCESS;

  status  = XMMRegs_FineDelayParameter_Set(InstancePtr, DELAYLINE_STEP_SIZE_DEFAULT, DELAYLINE_STEP_INTERVAL_DEFAULT);
  status |= XMMRegs_LmkPllDelayParameter_Set(InstancePtr, LMKPLL_STEP_SIZE_DEFAULT, LMKPLL_STEP_INTERVAL_DEFAULT);
  status |= XMMRegs_AlignMode_Set(InstancePtr, CARRIER_EXCLUDED);

  return status;
}

void XMMRegs_Settings_PrintAll(XMMRegs *InstancePtr)
{
  XMMRegs_Settings s = InstancePtr->Settings;
  XMMRegs_Identification_t t = InstancePtr->Identification;

  DBG(DBLI, "\nPrintAll of Settings :--------------------------------------------------------------\n");
  DBG(DBLI, "fineDelayStepSize\t:\t%u\n", s.fineDelayStepSize); 
  DBG(DBLI, "fineDelayStepInterval\t:\t%u\n", s.fineDelayStepInterval); 
  DBG(DBLI, "LmkPllDelayStepSize\t:\t%u\n", s.LmkPllDelayStepSize); 
  DBG(DBLI, "LmkPllDelayStepInterval\t:\t%u\n", s.LmkPllDelayStepInterval); 
  DBG(DBLI, "alignMode\t\t:\t%u\n", s.alignMode); 
  DBG(DBLI, "gts_version\t\t:\t%u\n", t.gts_version);
  DBG(DBLI, "carrier_type\t\t:\t%u\n", t.carrier_type);
  DBG(DBLI, "trigger_type\t\t:\t%u\n", t.trigger_type);
  DBG(DBLI, "rev_project\t\t:\t%u\n", t.rev_project);
  DBG(DBLI, "rev_specific_driver\t:\t%u\n", t.rev_specific_driver);
  DBG(DBLI, "rev_shared_driver\t:\t%u\n", t.rev_shared_driver);
}

/************************************************************

		DATA FUNCTIONS

************************************************************/

int XMMRegs_Data_TotalDelay_Set(XMMRegs *InstancePtr, unsigned int total_delay) {
  InstancePtr->Data.total_delay = total_delay;
  return XST_SUCCESS;
}

unsigned int XMMRegs_Data_TotalDelay_Read(XMMRegs *InstancePtr) {
  return InstancePtr->Data.total_delay;
}

int  XMMRegs_Data_Setup(XMMRegs *InstancePtr) {
  InstancePtr->Data.total_delay = XMMRegs_LmkPll_FineDelay_Read(InstancePtr) + XMMRegs_MuxInOut_FineDelay_Read(InstancePtr);
  return XST_SUCCESS;
}

int  XMMRegs_Data_Start(XMMRegs *InstancePtr) {
  return XST_SUCCESS;
}

int  XMMRegs_Data_Stop(XMMRegs *InstancePtr) {
  InstancePtr->Data.total_delay = XMMRegs_LmkPll_FineDelay_Read(InstancePtr) + XMMRegs_MuxInOut_FineDelay_Read(InstancePtr);
  return XST_SUCCESS;
}

void XMMRegs_Data_PrintAll(XMMRegs *InstancePtr)
{
  XMMRegs_Data_t s = InstancePtr->Data;

  DBG(DBLI, "\nPrintAll of Data :--------------------------------------------------------------\n");
  DBG(DBLI, "total delay\t\t:\t%u\n", s.total_delay); 
  DBG(DBLI, "lmk_config_CLKout4\t:\t%08X\n", s.lmk_config_CLKout4); 
}
    
/************************************************************

	      RESET, INIT, PRINT FUNCTIONS

************************************************************/

XMMRegs_Config *XMMRegs_LookupConfig(unsigned short DeviceId)
{
  XMMRegs_Config *CfgPtr = NULL;

  if (XMMRegs_ConfigTable[0].DeviceId == DeviceId)
  {
    CfgPtr = &XMMRegs_ConfigTable[0];
  }

  return CfgPtr;
}

int XMMRegs_Initialize(XMMRegs *InstancePtr, unsigned short DeviceId)
{
  XMMRegs_Config *CfgPtr;

  CfgPtr = XMMRegs_LookupConfig(DeviceId);

  if (CfgPtr == (XMMRegs_Config*)NULL) return XST_DEVICE_NOT_FOUND;

  InstancePtr->BaseAddress = CfgPtr->BaseAddress;

  XMMRegs_ClearStats(InstancePtr);

  return XST_SUCCESS;
}

int XMMRegs_Setup(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  XMMRegs_ClearStats(InstancePtr);

  status  = XMMRegs_Status_Setup(InstancePtr);
  status |= XMMRegs_Settings_Setup(InstancePtr);
  status |= XMMRegs_MuxInOut_Setup(InstancePtr);
  status |= XMMRegs_ClockPath_Setup(InstancePtr, XMMR_SETUP);
  status |= XMMRegs_DataPath_Setup(InstancePtr);
  status |= XMMRegs_Trigger_Setup(InstancePtr);
  XMMRegs_RocketIO_Setup(InstancePtr);
  status |= XMMRegs_Data_Setup(InstancePtr);
  status |= XMMRegs_Reg_Setup(InstancePtr);

  return status;  
}

int XMMRegs_Start(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Status_Start(InstancePtr);
  status |= XMMRegs_Settings_Start(InstancePtr);
  status |= XMMRegs_MuxInOut_Start(InstancePtr);
  status |= XMMRegs_ClockPath_Start(InstancePtr);
  status |= XMMRegs_DataPath_Start(InstancePtr);
  status |= XMMRegs_Trigger_Start(InstancePtr);
  XMMRegs_RocketIO_Start(InstancePtr);
  status |= XMMRegs_Data_Start(InstancePtr);
  status |= XMMRegs_Reg_Start(InstancePtr);

  return status;
}

int XMMRegs_Stop(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  XMMRegs_RocketIO_Stop(InstancePtr);
  status  = XMMRegs_Trigger_Stop(InstancePtr);
  status |= XMMRegs_DataPath_Stop(InstancePtr);
  status |= XMMRegs_ClockPath_Stop(InstancePtr);
  status |= XMMRegs_MuxInOut_Stop(InstancePtr);
  status |= XMMRegs_Settings_Stop(InstancePtr);
  status |= XMMRegs_Status_Stop(InstancePtr);
  status |= XMMRegs_Data_Stop(InstancePtr);
  status |= XMMRegs_Reg_Stop(InstancePtr);

  XMMRegs_ClearStats(InstancePtr);

  return status;
}

int XMMRegs_Reset(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Stop(InstancePtr);
  status |= XMMRegs_Init(InstancePtr);

  return status;
}

int XMMRegs_Init(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Setup(InstancePtr);
  status |= XMMRegs_Start(InstancePtr);
  status |= XMMRegs_Status_Check(InstancePtr);

  return status;
}

/* XMMRegs_Update reinitializes all the drivers
but it doesn't setup the StatusTransceiver vector beforehand.
It has to be launched after the user has set correctly
the StatusTransceiver vector */
int  XMMRegs_Update(XMMRegs *InstancePtr)
{
  int status = XST_SUCCESS;

  status  = XMMRegs_Status_Check(InstancePtr);

  status |= XMMRegs_Settings_Setup(InstancePtr);
  status |= XMMRegs_MuxInOut_Setup(InstancePtr);
  status |= XMMRegs_ClockPath_Setup(InstancePtr, XMMR_UPDATE);
  status |= XMMRegs_DataPath_Setup(InstancePtr);
  status |= XMMRegs_Trigger_Setup(InstancePtr);
  XMMRegs_RocketIO_Setup(InstancePtr);
  status |= XMMRegs_Data_Setup(InstancePtr); 
  status |= XMMRegs_Reg_Setup(InstancePtr);

  status |= XMMRegs_Settings_Start(InstancePtr);
  status |= XMMRegs_MuxInOut_Start(InstancePtr);
  status |= XMMRegs_ClockPath_Start(InstancePtr);
  status |= XMMRegs_DataPath_Start(InstancePtr);
  status |= XMMRegs_Trigger_Start(InstancePtr);
  XMMRegs_RocketIO_Start(InstancePtr);
  status |= XMMRegs_Data_Start(InstancePtr); 
  status |= XMMRegs_Reg_Start(InstancePtr);

  return status;
}

int XMMRegs_SelfTest(XMMRegs *InstancePtr)
{
  return XST_SUCCESS;
}

void XMMRegs_GetStats(XMMRegs *InstancePtr, XMMRegs_Stats *Stats)
{
  *Stats = InstancePtr->Stats;
  return;
}

void XMMRegs_ClearStats(XMMRegs *InstancePtr)
{
  int i;
  unsigned char *p = (unsigned char *)(&InstancePtr->Stats);
  for(i=0; i<sizeof(XMMRegs_Stats); i++) p[i]=0;

  return;
}

void XMMRegs_PrintAll(XMMRegs *InstancePtr)
{
  XMMRegs_Status_PrintAll(InstancePtr);
  XMMRegs_Settings_PrintAll(InstancePtr);
  XMMRegs_Data_PrintAll(InstancePtr);
  XMMRegs_MuxInOut_PrintAll(InstancePtr);
  XMMRegs_DataPath_PrintAll(InstancePtr);
//  XMMRegs_TdcDebug_PrintAll(InstancePtr);
  XMMRegs_Trigger_PrintAll(InstancePtr);
  XMMRegs_RocketIO_PrintAll(InstancePtr);
  XMMRegs_ClockPath_PrintAll(InstancePtr);
//  XMMRegs_LmkPll_PrintAll(InstancePtr);

  return;
}

#undef DBG
