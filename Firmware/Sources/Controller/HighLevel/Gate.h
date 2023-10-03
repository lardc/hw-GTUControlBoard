// -----------------------------------------
// Logic of Gate testing
// ----------------------------------------

#ifndef __GATE_H
#define __GATE_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "Controller.h"

// Functions
//
// Prepare measurements
void GATE_Prepare();
// Process Gate measurements
Boolean GATE_Process(CombinedData Sample, pDeviceStateCodes Codes);

#endif // __GATE_H
