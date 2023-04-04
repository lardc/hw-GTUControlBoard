// -----------------------------------------
// Driver for EEPROM via SPI
// ----------------------------------------

#ifndef __ZBMEMORY_H
#define __ZBMEMORY_H

// Include
#include "stdinc.h"
#include "ZwDSP.h"


// Functions
//
// Init SRAM interfaces
void ZbMEM_Init();
// Write values to EPROM
void ZbMEM_WriteValuesEPROM(Int16U EPROMAddress, pInt16U Buffer, Int16U BufferSize);
// Read values from EPROM
void ZbMEM_ReadValuesEPROM(Int16U EPROMAddress, pInt16U Buffer, Int16U BufferSize);


#endif // __ZBMEMORY_H
