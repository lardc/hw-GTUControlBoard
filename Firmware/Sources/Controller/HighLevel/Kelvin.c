// ----------------------------------------
// Controller of Kelvin block
// ----------------------------------------

// Header
#include "Kelvin.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"


// Types
//
typedef struct __BridgeState
{
	Int16U High, Low;
} BridgeState;


// Constants
//
#define KELVIN_PROBE_COUNT					4
#define KELVIN_BRIDGE_ARMS					4
//
// Tests: CC -> CP = 1, AP -> CP = 0, CP -> AP = 0, AC -> CC = 1
const BridgeState BridgeContacts[KELVIN_PROBE_COUNT + 1] = {{1, 3}, {4, 3}, {3, 4}, {2, 1}, {0, 0}};
// CP = K3, CC = K1, AC = K2, AP = K4
const Int16U BridgeHigh[KELVIN_BRIDGE_ARMS] = {0x80u, 0x20u, 0x08u, 0x02u};
const Int16U BridgeLow[KELVIN_BRIDGE_ARMS] = {0x01u, 0x40u, 0x10u, 0x04u};
// Pattern of correct results
const Boolean ResultsPattern[KELVIN_PROBE_COUNT] = {TRUE, FALSE, FALSE, TRUE};


// Variables
//
static Int16U InternalProbeCounter, ProbeIndex, ProbeTime;
static Boolean Results[KELVIN_PROBE_COUNT];


// Forward functions
//
void KELVIN_SwitchBridge(BridgeState State);
void KELVIN_SetProbeResult(pInt16U Result);


// Functions
//
void KELVIN_Prepare()
{
	ZbDAC_ForceOutputsToZero();

	InternalProbeCounter = 0;
	ProbeIndex = 0;
	ProbeTime = 1000 / TIMER0_PERIOD * DataTable[REG_KELVIN_PROBE_TIME];

	KELVIN_SwitchBridge(BridgeContacts[ProbeIndex]);
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean KELVIN_Process(pDeviceStateCodes Codes)
{
	++InternalProbeCounter;

	if (InternalProbeCounter == ProbeTime)
	{
		InternalProbeCounter = 0;
		Results[ProbeIndex] = ZbGPIO_KelvinProbe();
		KELVIN_SwitchBridge(BridgeContacts[++ProbeIndex]);
	}

	if (ProbeIndex == KELVIN_PROBE_COUNT)
	{
		Int16U i;
		Boolean total = TRUE;
		
		for(i = 0; i < KELVIN_PROBE_COUNT; ++i)
		{
			total = total && (Results[i] == ResultsPattern[i]);
			DataTable[REG_KELVIN_1_2 + i] = Results[i] ? 1 : 0;
		}
		DataTable[REG_RESULT_KELVIN] = total ? 1 : 0;
		DataTable[REG_PROCESS_COUNTER] = InternalProbeCounter;
		ZwTimer_StopT0();
		return TRUE;
	}
	
	return FALSE;
}
// ----------------------------------------

void KELVIN_SwitchBridge(BridgeState State)
{
	Int16U Bridge = 0;

	if ((State.Low > 0) && (State.Low <= KELVIN_BRIDGE_ARMS))
		Bridge |= BridgeLow[State.Low - 1];	
	if ((State.High > 0) && (State.High <= KELVIN_BRIDGE_ARMS))
		Bridge |= BridgeHigh[State.High - 1];

	ZbVRegister_WriteRegister(Bridge);
}
// ----------------------------------------

void KELVIN_Diag(Int16U Index)
{
	ZbDAC_ForceOutputsToZero();
	KELVIN_SwitchBridge(BridgeContacts[(Index < KELVIN_PROBE_COUNT) ? Index : KELVIN_PROBE_COUNT]);
}
// ----------------------------------------
