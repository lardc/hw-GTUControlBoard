// -----------------------------------------
// Project custom types
// ----------------------------------------

#ifndef __PROJ_TYPES_H
#define __PROJ_TYPES_H

#include "IQmathLib.h"

typedef struct __CombinedData
{
	_iq Vg;
	_iq Ig;
	_iq Vd;
	_iq Id;
} CombinedData, *pCombinedData;

#endif // __PROJ_TYPES_H
