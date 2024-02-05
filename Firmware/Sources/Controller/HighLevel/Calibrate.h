// ----------------------------------------
// Logic of Gate calibration
// ----------------------------------------

#ifndef __CALIBRATE_H
#define __CALIBRATE_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "Controller.h"
#include "Regulator.h"

// Functions
//
// Prepare measurements
void CALIBRATE_Prepare(RegulatorSelector Select);
// Process Gate measurements
Boolean CALIBRATE_Process(CombinedData MeasureSample, pDeviceStateCodes Codes);

#endif // __CALIBRATE_H
