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
void ZbGPIO_DirectEnableSelfTest(Boolean State);
void ZbGPIO_DirectEnableOutput(Boolean State);
void ZbGPIO_DirectLockFeedback(Boolean State);
//
void ZbGPIO_GateEnableSelfTest(Boolean State);
void ZbGPIO_GateEnableOutput(Boolean State);
void ZbGPIO_GateLockFeedback(Boolean State);
//
Boolean ZbGPIO_KelvinProbe();
//
void ZbGPIO_LED1(Boolean State);
void ZbGPIO_LED2(Boolean State);
void ZbGPIO_ToggleLED2();
//
Boolean ZbGPIO_ExtSyncReadState();

#endif // __ZBGPIO_H
