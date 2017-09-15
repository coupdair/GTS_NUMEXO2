#ifndef XMMREGS_H
#define XMMREGS_H

#include "xstatus.h"
#include "xmmregs_l.h"

#define GTS_VERSION         3
#define REV_PROJECT         1
#define REV_SPECIFIC_DRIVER 1
#define REV_SHARED_DRIVER   1

#define TRANSCEIVER_IS_CONNECTED                  1
#define TRANSCEIVER_IS_NOT_CONNECTED              0
#define TRANSCEIVER_IS_CONNECTED_TO_DIGITIZER     1
#define TRANSCEIVER_IS_NOT_CONNECTED_TO_DIGITIZER 0
#define TRANSCEIVER_IS_IN_GTS_TREE                1
#define TRANSCEIVER_IS_NOT_IN_GTS_TREE            0

#define CARRIER_EXCLUDED 0  /* CARRIER_EXCLUDED : the alignment is done without including the carrier */
#define CARRIER_INCLUDED 1 /* CARRIER_INCLUDED : the alignment is done including the carrier */

#define XMMR_SETUP  1 /* initial setup -> used for ClockPath */
#define XMMR_UPDATE 2 /* update setup  -> used for ClockPath */

typedef struct
{
  unsigned short DeviceId;		/**< Unique ID  of device */
  void *BaseAddress;
} XMMRegs_Config;

typedef struct
{
  unsigned int ReceivedBytes;
  unsigned int SentBytes;
  unsigned int MissingTargets;
} XMMRegs_Stats;

typedef struct
{
  unsigned int fineDelayStepSize;
  unsigned int fineDelayStepInterval;
  unsigned int LmkPllDelayStepSize;
  unsigned int LmkPllDelayStepInterval;
  unsigned int alignMode; /* CARRIER_INCLUDED : the alignment is done including the carrier */
  /* CARRIER_EXCLUDED : the alignment is done without including the carrier for the leave GTS */
} XMMRegs_Settings; 

typedef struct
{
  unsigned int gts_version;
  unsigned int carrier_type;
  unsigned int trigger_type;
  unsigned int rev_project;
  unsigned int rev_specific_driver;
  unsigned int rev_shared_driver;
} XMMRegs_Identification_t;

typedef struct
{
  unsigned int total_delay; /* total delay (delay_line + Lmk_Pll) in units of delay line */
  unsigned int lmk_config_CLKout4; /* configuration vector sent to LMK_PLL to change the Lmk_Pll delay */
} XMMRegs_Data_t;

typedef struct
{
  unsigned int             : 20;
  unsigned int digitizer   :  4; /* if equal to 0xF, all transceivers are connected to the digitizer */
  unsigned int gts_tree    :  4; /* if equal to 0xF, all transceivers belong to the gts tree */
  unsigned int connected   :  4; /* if equal to 0xF, all transceivers are considered to be connected */
                        /* if equal to 1, only transceiver 0 is considered connected */
                        /* if equal to 0, no transceiver is considered connected */
} XMMRegs_StatusTransceiver;

typedef struct
{
  char *name;
  unsigned int val;
} XMMRegs_Bitfield_t;

typedef struct
{
  char *name;
  unsigned int offset;
  unsigned int *val;
  XMMRegs_Bitfield_t *bitfield;
} XMMRegs_Reg_t;

typedef struct
{
  XMMRegs_Stats Stats;
  XMMRegs_StatusTransceiver Status;
  XMMRegs_Settings Settings;
  XMMRegs_Identification_t Identification;
  XMMRegs_Data_t Data;
  void *BaseAddress;
  XMMRegs_Reg_t *reg;
} XMMRegs;

/* standard API */
extern int  XMMRegs_Initialize(XMMRegs *InstancePtr, unsigned short DeviceId);
extern int  XMMRegs_Setup(XMMRegs *InstancePtr);
extern int  XMMRegs_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_Reset(XMMRegs *InstancePtr);
extern int  XMMRegs_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_Update(XMMRegs *InstancePtr);

extern int  XMMRegs_SelfTest(XMMRegs *InstancePtr);
extern void XMMRegs_GetStats(XMMRegs *InstancePtr, XMMRegs_Stats *Stats);
extern void XMMRegs_ClearStats(XMMRegs *InstancePtr);

/* low level custom API */
extern void XMMRegs_PrintBinary(XMMRegs *InstancePtr, unsigned int offset);
extern void XMMRegs_PrintHex(XMMRegs *InstancePtr, unsigned int offset);
extern unsigned int XMMRegs_ReadRegister(XMMRegs *InstancePtr, unsigned int offset);

/* high level custom API */
extern void XMMRegs_PrintAll(XMMRegs *InstancePtr);

/***********************************************/
/*  Reg APIs                                   */
/***********************************************/
extern int XMMRegs_Reg_Associate(XMMRegs *InstancePtr, int config);
extern int XMMRegs_Reg_ReadAll(XMMRegs *InstancePtr);
extern int XMMRegs_Reg_ReadReg(XMMRegs *InstancePtr, char *reg_name);
extern int XMMRegs_Reg_ReadField(XMMRegs *InstancePtr, char *reg_name, char *field_name);
extern int XMMRegs_Reg_WriteField(XMMRegs *InstancePtr, char *reg_name, char *field_name, unsigned int val_field);
extern int XMMRegs_Reg_Associate(XMMRegs *InstancePtr, int config);
extern int XMMRegs_Reg_Write(XMMRegs *InstancePtr);
extern int XMMRegs_Reg_Setup(XMMRegs *InstancePtr);
extern int XMMRegs_Reg_Start(XMMRegs *InstancePtr);
extern int XMMRegs_Reg_Stop(XMMRegs *InstancePtr);
extern int XMMRegs_Reg_Init(XMMRegs *InstancePtr);
extern int XMMRegs_Reg_Reset(XMMRegs *InstancePtr);

/***********************************************/
/*  Status APIs                                */
/***********************************************/
extern int  XMMRegs_Status_Setup(XMMRegs *InstancePtr);
extern int  XMMRegs_Status_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_Status_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_Status_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_Status_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_Status_PrintAll(XMMRegs *InstancePtr);
extern int  XMMRegs_Status_Check(XMMRegs *InstancePtr);
extern int  XMMRegs_IsTransceiverConnected(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_Transceiver_Connect(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_Transceiver_DisConnect(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_IsTransceiverInGtsTree(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_TransceiverGtsTree_Insert(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_TransceiverGtsTree_Remove(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_IsTransceiverConnectedToDigitizer(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_TransceiverDigitizer_Connect(XMMRegs *InstancePtr, int transceiver);
extern int  XMMRegs_TransceiverDigitizer_DisConnect(XMMRegs *InstancePtr, int transceiver);

/***********************************************/
/*  Settings APIs                              */
/***********************************************/
extern int  XMMRegs_Settings_Setup(XMMRegs *InstancePtr);
extern int  XMMRegs_Settings_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_Settings_Stop(XMMRegs *InstancePtr);
extern void XMMRegs_Settings_PrintAll(XMMRegs *InstancePtr);
extern int  XMMRegs_FineDelayParameter_Set(XMMRegs *InstancePtr, unsigned int step_size, unsigned int step_interval);
extern int  XMMRegs_AlignMode_Set(XMMRegs *InstancePtr, unsigned int clk_used);
extern unsigned int  XMMRegs_AlignMode_Read(XMMRegs *InstancePtr);

/***********************************************/
/*  Data APIs                              */
/***********************************************/
extern int  XMMRegs_Data_TotalDelay_Set(XMMRegs *InstancePtr, unsigned int total_delay);
extern unsigned int  XMMRegs_Data_TotalDelay_Read(XMMRegs *InstancePtr);
extern int  XMMRegs_Data_Setup(XMMRegs *InstancePtr);
extern int  XMMRegs_Data_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_Data_Stop(XMMRegs *InstancePtr);
extern void XMMRegs_Data_PrintAll(XMMRegs *InstancePtr);

/***********************************************/
/*  MuxInOut APIs                              */
/***********************************************/
extern void XMMRegs_MuxInOut_Setup(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_Start(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_Stop(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_Init(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_PrintAll(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_PllMonitor_Reset(XMMRegs *InstancePtr);
extern int  XMMRegs_MuxInOut_IsPllLocked(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_ExternalRxMux_Set(XMMRegs *InstancePtr, int b);
extern void XMMRegs_MuxInOut_ExternalTxMux_Set(XMMRegs *InstancePtr, int b);
extern void XMMRegs_MuxInOut_Tdc_Enable(XMMRegs *InstancePtr);
extern void XMMRegs_MuxInOut_Tdc_Disable(XMMRegs *InstancePtr);
extern int  XMMRegs_MuxInOut_FineDelay_Set(XMMRegs *InstancePtr, unsigned int delay);
extern unsigned int  XMMRegs_MuxInOut_FineDelay_Read(XMMRegs *InstancePtr);
extern int  XMMRegs_MuxInOut_FineDelay_GradualSet(XMMRegs *InstancePtr, unsigned int delay_before, unsigned int delay_after);
extern unsigned int  XMMRegs_MuxInOut_FromPsToFineDelay_Translate(XMMRegs *InstancePtr, int delay_in_ps);
extern int  XMMRegs_MuxInOut_FineDelayInPs_Read(XMMRegs *InstancePtr);

/***********************************************/
/*  TdcDebug APIs                              */
/***********************************************/
extern int  XMMRegs_TdcDebug_Setup(XMMRegs *InstancePtr);
extern int  XMMRegs_TdcDebug_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_TdcDebug_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_TdcDebug_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_TdcDebug_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_TdcDebug_PrintAll(XMMRegs *InstancePtr);
extern int  XMMRegs_TdcDebug_OneSweep_Do(XMMRegs *InstancePtr);
extern int  XMMRegs_TdcDebug_Delay_Set(XMMRegs *InstancePtr, unsigned int delay);
extern int  XMMRegs_TdcDebug_DebugMode_Launch(XMMRegs *InstancePtr);
extern int  XMMRegs_TdcDebug_DebugMode_Remove(XMMRegs *InstancePtr);

/***********************************************/
/*  ClockPath APIs                             */
/***********************************************/
extern int  XMMRegs_ClockPath_Setup(XMMRegs *InstancePtr, int setup_mode);
extern int  XMMRegs_ClockPath_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_ClockPath_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_ClockPath_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_ClockPath_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_ClockPath_PrintAll(XMMRegs *InstancePtr);
extern int  XMMRegs_ClockPath_UserLogicClock_Set(XMMRegs *InstancePtr, unsigned char choice_clk);
extern int  XMMRegs_ClockPath_RawClock_Set(XMMRegs *InstancePtr, unsigned char choice_clk);
extern int  XMMRegs_ClockPath_RawClock_Diagnose(XMMRegs *InstancePtr);
extern int  XMMRegs_ClockPath_IsMictorDcmLocked(XMMRegs *InstancePtr);
extern int  XMMRegs_ClockPath_IsFbDcmLocked(XMMRegs *InstancePtr);
extern int  XMMRegs_ClockPath_MictorDcm_Launch(XMMRegs *InstancePtr);
 
/***********************************************/
/*  RocketIO APIs                           */
/***********************************************/
extern void XMMRegs_RocketIO_Setup(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_Start(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_Stop(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_Init(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_TriggerRstCarrier_Set(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_TriggerRstCarrier_Unset(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_PrintAll(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_MgtData_Set(XMMRegs *InstancePtr, unsigned int msb, unsigned int lsb, unsigned char comma_msb, unsigned char comma_lsb);
extern void XMMRegs_RocketIO_RefClk_Set(XMMRegs *InstancePtr);
extern int  XMMRegs_RocketIO_CommaAlign_Diagnose(XMMRegs *InstancePtr);
extern unsigned int  XMMRegs_RocketIO_RxMgtdata_Read(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_TxSystem_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_RxSystem_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_TxSystem_Stop(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_RxSystem_Stop(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_TxSystem_Init(XMMRegs *InstancePtr);
extern void XMMRegs_RocketIO_RxSystem_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_RocketIO_TxSystem_Status(XMMRegs *InstancePtr);
extern int  XMMRegs_RocketIO_RxSystem_Status(XMMRegs *InstancePtr);
  
/***********************************************/
/*  DataPath APIs                              */
/***********************************************/
extern int  XMMRegs_DataPath_Setup(XMMRegs *InstancePtr);
extern void XMMRegs_DataPath_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_DataPath_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_DataPath_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_DataPath_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_DataPath_PrintAll(XMMRegs *InstancePtr);
extern int  XMMRegs_DataPath_MuxClkSync_Set(XMMRegs *InstancePtr, unsigned int val);
extern void XMMRegs_DataPath_AllTxmux_Set(XMMRegs *InstancePtr, int val);
extern void XMMRegs_DataPath_AllRxmux_Set(XMMRegs *InstancePtr, int val);
extern int  XMMRegs_DataPath_CoarseDelay_Set(XMMRegs *InstancePtr, unsigned int delay);
extern unsigned int  XMMRegs_DataPath_CoarseDelay_Read(XMMRegs *InstancePtr);
extern void XMMRegs_DataPath_UseSync_Set(XMMRegs *InstancePtr);
extern void XMMRegs_DataPath_UseSync_UnSet(XMMRegs *InstancePtr);
extern int  XMMRegs_DataPath_UseSync_MasterToMaster_Set(XMMRegs *InstancePtr, int forward);
extern int  XMMRegs_DataPath_UseSync_MasterToSlave_Set(XMMRegs *InstancePtr, int forward);
extern int  XMMRegs_DataPath_UseSync_SyncToSlave_Set(XMMRegs *InstancePtr, int forward);
extern void XMMRegs_DataPath_OneSweep_Do(XMMRegs *InstancePtr);
extern void XMMRegs_DataPath_Digitizer_Connect(XMMRegs *InstancePtr);
extern void XMMRegs_DataPath_Digitizer_Disconnect(XMMRegs *InstancePtr);

/***********************************************/
/*  Trigger APIs                               */
/***********************************************/
extern int  XMMRegs_Trigger_Setup(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_Trigger_PrintAll(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_CoreType_Get(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_CrystalID_Set(XMMRegs *InstancePtr, int crystalID);
extern void XMMRegs_Trigger_GtsReady_Set(XMMRegs *InstancePtr);
extern void XMMRegs_Trigger_GtsReady_UnSet(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Leave_TestLoopback_Set(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Leave_Test_Set(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Leave_CarrierRequest_Set(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Root_Loopback_Set(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Root_AuroraLoopback_Set(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Daq_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_Daq_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_UserDcm_Launch(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_CarrierLmkPllLock_Read(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_CarrierType_Read(XMMRegs *InstancePtr);
extern int  XMMRegs_Trigger_PeriodMask_Set(XMMRegs *InstancePtr, int log2_period);

/***********************************************/
/*  external PLL APIs                          */
/***********************************************/
extern int  XMMRegs_LmkPll_WriteReg(XMMRegs *InstancePtr, unsigned int wdata);
extern int  XMMRegs_LmkPll_LoadRegConfig_Default(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_LoadRegConfig_Change(XMMRegs *InstancePtr, int PLL_R, int PLL_N, int VCO_DIV);
extern int  XMMRegs_LmkPll_ConfigReset(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_WriteRaw(XMMRegs *InstancePtr, unsigned int address, unsigned int data);
extern int  XMMRegs_LmkPll_LoadRawConfig_Default(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_IsLocked(XMMRegs *InstancePtr);
extern void XMMRegs_LmkPll_GlobalOutputDisable(XMMRegs *InstancePtr);
extern void XMMRegs_LmkPll_GlobalOutputEnable(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_FineDelay_Set(XMMRegs *InstancePtr, unsigned int delay); 
extern int  XMMRegs_LmkPll_FineDelay_LogSet(XMMRegs *InstancePtr, unsigned int delay);
extern int  XMMRegs_LmkPll_FineDelay_GradualSet(XMMRegs *InstancePtr, unsigned int old_delay, unsigned int new_delay);
extern unsigned int  XMMRegs_LmkPll_FineDelay_Read(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_Setup(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_Start(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_Stop(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_Init(XMMRegs *InstancePtr);
extern int  XMMRegs_LmkPll_Reset(XMMRegs *InstancePtr);
extern void XMMRegs_LmkPll_PrintAll(XMMRegs *InstancePtr);

#endif
