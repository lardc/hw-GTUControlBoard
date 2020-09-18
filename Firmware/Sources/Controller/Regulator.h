// ----------------------------------------
// Universal regulator
// ----------------------------------------

#ifndef __REGULATOR_H
#define __REGULATOR_H

// Include
#include "stdinc.h"
//
#include "IQmathLib.h"
#include "MeasureUtils.h"


// Types
//
typedef enum __RegulatorSelector
{
	SelectVd = 0,
	SelectId,
	SelectVg,
	SelectIg
} RegulatorSelector;


// Functions
//
void REGULATOR_Cycle(CombinedData MeasureSample);

void REGULATOR_Enable(RegulatorSelector Selector, Boolean State);
void REGULATOR_Update(RegulatorSelector Selector, _iq Value);
void REGULATOR_SetOutput(RegulatorSelector Selector, _iq Value);
Boolean REGULATOR_IsIErrorSaturated(RegulatorSelector Selector);

void REGULATOR_InitAll();
void REGULATOR_DisableAll();

CombinedData REGULATOR_GetControl();
CombinedData REGULATOR_GetTarget();

#endif // __REGULATOR_H
