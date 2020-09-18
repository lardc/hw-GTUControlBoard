// -----------------------------------------
// Logic of latching current testing process
// ----------------------------------------

#ifndef __LATCHING_H
#define __LATCHING_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "Controller.h"

// Functions
//
// Prepare measurements
void LATCHING_Prepare();
// Process LC measurements
Boolean LATCHING_Process(CombinedData MeasureSample, pDeviceStateCodes Codes);

#endif // __LATCHING_H
