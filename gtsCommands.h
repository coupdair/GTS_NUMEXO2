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

	ExcludeCarrierForAlign, AlignStart, FineDelaySetNow,
	FineDelaySet, CoarseDelaySet, AlignSet, AlignTdcSet,

	PrintBinary, SetOutputFileName, AlignMeas, TdcSet, TdcMeas,

	TriggerSetup, TriggerStart, ExcludeTriggerProcessor, TestSet,

	ReadField,

	MaxGtsCommands
};

extern	GTScommands	gtsCommands [MaxGtsCommands];

#endif	// __GTSCOMMANDS_H_
