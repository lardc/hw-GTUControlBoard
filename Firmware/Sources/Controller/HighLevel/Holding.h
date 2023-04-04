// ----------------------------------------
// Controller of Holding current testing
// ----------------------------------------

#ifndef __HOLDING_H
#define __HOLDING_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "Controller.h"

// Functions
//
// Prepare measurements
void HOLDING_Prepare();
// Process IH measurements
Boolean HOLDING_Process(CombinedData Sample, pDeviceStateCodes Codes);

#endif // __HOLDING_H
