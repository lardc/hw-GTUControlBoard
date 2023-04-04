// -----------------------------------------
// Controller of Kelvin block
// ----------------------------------------

#ifndef __KELVIN_H
#define __KELVIN_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "Controller.h"

// Functions
//
// Prepare measurements
void KELVIN_Prepare();
// Process Kelvin measurements
Boolean KELVIN_Process(pDeviceStateCodes Codes);
// Set manual Kelvin state
void KELVIN_Diag(Int16U Index);

#endif // __KELVIN_H
