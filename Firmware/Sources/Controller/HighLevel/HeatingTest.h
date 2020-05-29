// -----------------------------------------
// Logic of Gate calibration
// ----------------------------------------

#ifndef __HEATING_TEST_H
#define __HEATING_TEST_H

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
void HEATING_TEST_Prepare();
// Process heating test
Boolean HEATING_TEST_Process(CombinedData MeasureSample, pDeviceStateCodes Codes);

#endif // __HEATING_TEST_H
