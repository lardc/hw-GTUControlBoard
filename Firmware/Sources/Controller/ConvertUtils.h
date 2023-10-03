// -----------------------------------------
// Utility functions for parameters conversion
// ----------------------------------------

#ifndef __CONVERT_UTIL_H
#define __CONVERT_UTIL_H

// Include
#include "stdinc.h"
//
#include "IQmathLib.h"


// Functions
//
void CU_Cache();
Int16U CU_GateVtoDAC(_iq Value);
Int16U CU_GateItoDAC(_iq Value);
Int16U CU_DirectVtoDAC(_iq Value);
Int16U CU_DirectItoDAC(_iq Value);


#endif // __CONVERT_UTIL_H
