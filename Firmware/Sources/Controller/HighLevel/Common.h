// -----------------------------------------
// Common high-level tools
// ----------------------------------------

#ifndef __COMMON_H
#define __COMMON_H

// Include
#include "stdinc.h"
#include "IQmathLib.h"
#include "IQmathUtils.h"

// Types
typedef struct __ChannelSettings
{
	_iq Setpoint;
	_iq Limit;
	_iq ChangeStep;
} ChannelSettings, *pChannelSettings;
typedef struct __CommonSettings
{
	Int32U CycleCounter;
	Int16U StabCounter;
	_iq IdLeak;
	_iq AllowedError;
	_iq VgMinInput;
} CommonSettings, *pCommonSettings;

// Functions
//
void COMMON_PrepareStart();
void COMMON_PrepareFinish();
void COMMON_Finish();
void COMMON_CacheVariables(pChannelSettings _Vd, pChannelSettings _Id, pChannelSettings _Vg, pChannelSettings _Ig,
						   pCommonSettings _LogicSettings);
void COMMON_CacheCalibrationVariables(pChannelSettings _Vd, pChannelSettings _Id, pChannelSettings _Vg, pChannelSettings _Ig,
						   pCommonSettings _LogicSettings);

#endif // __COMMON_H
