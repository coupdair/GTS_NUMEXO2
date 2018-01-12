#ifndef __GTSCOMMANDS_H_
#define __GTSCOMMANDS_H_
/////////////////////
// GTS Command Set //
/////////////////////
enum GtsCommands {
	GtsTest,

	TelnetResume, UdpServer_Free,

	ProgTruncatedNumber, ClientReset, GtsReset,

	TransCon, TransGtsTree, TransDigitizer, GtsUpdate, ConIs, GtsIs,

	TreeSetup, TreeStart, RxSystemIsReady, TreeRead, TreeStop,

	ExcludeCarrierForAlign, IncludeCarrierForAlign, AlignStart, FineDelaySetNow,
	FineDelaySet, CoarseDelaySet, AlignSet, AlignTdcSet,

	PrintBinary, SetOutputFileName, AlignMeas, TdcSet, TdcMeas,

	TriggerSetup, TriggerStart, ExcludeTriggerProcessor, TestSet, LoopbackSet,

	ReadField, ReadReg, ReadAll, WriteField,

	MaxGtsCommands
};

extern	GTScommands	gtsCommands [MaxGtsCommands];

#endif	// __GTSCOMMANDS_H_
