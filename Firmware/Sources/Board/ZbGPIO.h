// -----------------------------------------
// Board-specific GPIO functions
// ----------------------------------------

#ifndef __ZBGPIO_H
#define __ZBGPIO_H

// Include
#include "stdinc.h"
#include "ZwDSP.h"

// Functions
//
void ZbGPIO_Init();
//
void ZbGPIO_DirectEnableOutput(Boolean State);
//
void ZbGPIO_GateEnableOutput(Boolean State);
//
Boolean ZbGPIO_KelvinProbe();
//
void ZbGPIO_LED1(Boolean State);
void ZbGPIO_LED2(Boolean State);
void ZbGPIO_ToggleLED2();
void ZbGPIO_SyncOSC(Boolean State);
void ZbGPIO_GatePSTune(Boolean State);
void ZbGPIO_FAN(Boolean State);
//
Boolean ZbGPIO_ExtSyncTrig();

#endif // __ZBGPIO_H
