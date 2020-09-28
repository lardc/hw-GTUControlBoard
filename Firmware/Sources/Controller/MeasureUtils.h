// -----------------------------------------
// Utility functions for measurements
// ----------------------------------------

#ifndef __MEASURE_UTIL_H
#define __MEASURE_UTIL_H

// Include
#include "stdinc.h"
//
#include "IQmathLib.h"

// Functions
//
// Кэшированние переменных пересчёта
void MU_Cache();

// Обработка результатов оцифровки
CombinedData MU_HandleADC(pInt16U Samples);

// Запись значений в массивы
void MU_LogScope(CombinedData MeasureSample);

#endif // __MEASURE_UTIL_H
