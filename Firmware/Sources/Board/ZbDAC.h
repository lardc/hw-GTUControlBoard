// -----------------------------------------
// Driver for DAC via SPI
// ----------------------------------------

#ifndef __ZBDAC_H
#define __ZBDAC_H

// Include
#include "stdinc.h"
#include "ZwDSP.h"

// Functions
//
// Init DAC interface
void ZbDAC_Init();
// Write to gate DAC
void ZbDAC_GateWriteVoltage(Int16U Voltage);
void ZbDAC_GateWriteCurrent(Int16U Current);
// Write to power DAC
void ZbDAC_DirectWriteVoltage(Int16U Voltage);
void ZbDAC_DirectWriteCurrent(Int16U Current);
void ZbDAC_ForceOutputsToZero();

#endif // __ZBDAC_H
