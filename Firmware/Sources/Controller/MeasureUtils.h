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
// ������������ ���������� ���������
void MU_Cache();

// ��������� ����������� ���������
CombinedData MU_HandleADC(pInt16U Samples);

// ������ �������� � �������
void MU_LogScope(CombinedData MeasureSample);

#endif // __MEASURE_UTIL_H
