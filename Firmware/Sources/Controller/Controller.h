// -----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __CONTROLLER_H
#define __CONTROLLER_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "MemBuffers.h"


// Constants
typedef enum __DeviceState
{
	DS_None			= 0,
	DS_Fault		= 1,
	DS_Disabled		= 2,
	DS_Kelvin		= 3,
	DS_Gate			= 4,
	DS_IH			= 5,
	DS_IL			= 6,
	DS_RG			= 7,
	DS_Calibrate	= 8,
	DS_Vgnt			= 9,
} DeviceState;
//
typedef struct __DeviceStateCodes
{
	Int16U Warning;
	Int16U Problem;
	Int16U FaultReason;
} DeviceStateCodes, *pDeviceStateCodes;


// Types
//
typedef void (*CONTROL_FUNC_RealTimeRoutine)();


// Variables
//
extern volatile Int64U CONTROL_TimeCounter;
extern volatile DeviceState CONTROL_State;
extern volatile Int16U CONTROL_Values_Counter;
extern volatile Int16U CONTROL_BootLoaderRequest;


// Functions
//
// Initialize controller
void CONTROL_Init(Boolean BadClockDetected);
// Update low-priority states
void CONTROL_Idle();
// Main high-priority control cycle
void CONTROL_UpdateHigh();
// Notify that CAN system fault occurs
void CONTROL_NotifyCANFault(ZwCAN_SysFlags Flag);
//
void FAN_Control();


#endif // __CONTROLLER_H
