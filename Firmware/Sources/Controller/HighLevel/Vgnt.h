// ----------------------------------------
// Logic of Gate testing
// ----------------------------------------

#ifndef __VGNT_H
#define __VGNT_H

// Include
#include "stdinc.h"
//
#include "ZwDSP.h"
#include "Global.h"
#include "Controller.h"

// Functions
//
// Prepare measurements
void VGNT_Prepare();
// Process Gate measurements
Boolean VGNT_Process(CombinedData Sample, pDeviceStateCodes Codes);

#endif // __VGNT_H
