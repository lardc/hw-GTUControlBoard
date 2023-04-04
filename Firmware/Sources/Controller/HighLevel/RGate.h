// ----------------------------------------
// Logic of Gate resistance testing
// ----------------------------------------

#ifndef __RGATE_H
#define __RGATE_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "Controller.h"

// Functions
//
// Prepare measurements
void RGATE_Prepare();
// Process Gate measurements
Boolean RGATE_Process(CombinedData MeasureSample, pDeviceStateCodes Codes);

#endif // __RGATE_H
