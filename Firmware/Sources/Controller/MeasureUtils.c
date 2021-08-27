// -----------------------------------------
// Utility functions for measurements
// ----------------------------------------

// Header
#include "MeasureUtils.h"
//
// Includes
#include "DeviceObjectDictionary.h"
#include "DataTable.h"
#include "Controller.h"
#include "stdlib.h"
#include "IQmathUtils.h"
#include "Regulator.h"


// Types
typedef struct __ADCConvParameters
{
	_iq K;
	_iq B;
	_iq P2;
	_iq P1;
	_iq P0;
} ADCConvParameters, *pADCConvParameters;


// Variables
static ADCConvParameters ParamsGateV, ParamsGateI, ParamsGateILow, ParamsDirectV, ParamsDirectI;
static _iq IgLowLimit;

// Forward functions
_iq MU_ADCtoX(Int16U ADCInput, ADCConvParameters Parameters);
ADCConvParameters MU_LoadParams(Int16U RegK, Int16U RegB, Int16U RegP2, Int16U RegP1, Int16U RegP0);

// Функции преобразования значений АЦП
_iq MU_GateV (Int16U ADCInput);
_iq MU_GateI (Int16U ADCInput);
_iq MU_DirectV(Int16U ADCInput);
_iq MU_DirectI(Int16U ADCInput);


// Functions
//
void MU_Cache()
{
	IgLowLimit = _IQI(DataTable[REG_DAC_LOW_IG_LIMIT]);
	// Цепь управления - напряжение
	ParamsGateV  = MU_LoadParams(REG_ADC_VG_CONV_K,  REG_ADC_VG_CONV_B,
								 REG_ADC_VG_FINE_P2, REG_ADC_VG_FINE_P1, REG_ADC_VG_FINE_P0);

	// Цепь управления - ток
	ParamsGateI  = MU_LoadParams(REG_ADC_IG_CONV_K,  REG_ADC_IG_CONV_B,
								 REG_ADC_IG_FINE_P2, REG_ADC_IG_FINE_P1, REG_ADC_IG_FINE_P0);

	// Цепь управления - младший ток до 50мА
	ParamsGateILow = MU_LoadParams(REG_ADC_LOW_IG_CONV_K,  REG_ADC_LOW_IG_CONV_B,
								   REG_ADC_LOW_IG_FINE_P2, REG_ADC_LOW_IG_FINE_P1, REG_ADC_LOW_IG_FINE_P0);

	// Силовая цепь - напряжение
	ParamsDirectV = MU_LoadParams(REG_ADC_VD_CONV_K, REG_ADC_VD_CONV_B,
								 REG_ADC_VD_FINE_P2, REG_ADC_VD_FINE_P1, REG_ADC_VD_FINE_P0);

	// Силовая цепь - ток
	ParamsDirectI = MU_LoadParams(REG_ADC_ID_CONV_K, REG_ADC_ID_CONV_B,
								 REG_ADC_ID_FINE_P2, REG_ADC_ID_FINE_P1, REG_ADC_ID_FINE_P0);
}
// ----------------------------------------

_iq MU_GateV(Int16U ADCInput)
{
	return MU_ADCtoX(ADCInput, ParamsGateV);
}
// ----------------------------------------

_iq MU_GateI(Int16U ADCInput)
{
	_iq IgtLow = MU_ADCtoX(ADCInput, ParamsGateILow);
	return (IgtLow < IgLowLimit) ? IgtLow : MU_ADCtoX(ADCInput, ParamsGateI);
}
// ----------------------------------------

_iq MU_DirectV(Int16U ADCInput)
{
	return MU_ADCtoX(ADCInput, ParamsDirectV);
}
// ----------------------------------------

_iq MU_DirectI(Int16U ADCInput)
{
	return MU_ADCtoX(ADCInput, ParamsDirectI);
}
// ----------------------------------------

ADCConvParameters MU_LoadParams(Int16U RegK, Int16U RegB, Int16U RegP2, Int16U RegP1, Int16U RegP0)
{
	ADCConvParameters ret;

	ret.K	= _FPtoIQ2(DataTable[RegK], 1000);
	ret.B	= _IQI((Int16S)DataTable[RegB]);
	//
	ret.P2	= _IQI((Int16S)DataTable[RegP2]);
	ret.P1	= _FPtoIQ2(DataTable[RegP1], 1000);
	ret.P0	= _IQI((Int16S)DataTable[RegP0]);

	return ret;
}
// ----------------------------------------

_iq MU_ADCtoX(Int16U ADCInput, ADCConvParameters Parameters)
{
	_iq tmp = _IQdiv((_IQI(ADCInput) - Parameters.B), Parameters.K);
	_iq tmp2 = _IQdiv(tmp, _IQ(1000.0f));

	_iq val = _IQmpy(tmp2, _IQmpy(tmp2, Parameters.P2)) + _IQmpy(tmp, Parameters.P1) + Parameters.P0;
	return (val > 0) ? val : 0;
}
// ----------------------------------------

CombinedData MU_HandleADC(pInt16U Samples)
{
	Int16U i, rawVg = 0, rawIg = 0, rawVd = 0, rawId = 0;
	CombinedData result;

	for (i = 0; i < 16; i += 4)
	{
		rawVg += Samples[i];
		rawIg += Samples[i + 1];
		rawVd += Samples[i + 2];
		rawId += Samples[i + 3];
	}

	result.Vg = MU_GateV  (rawVg >> 2);
	result.Ig = MU_GateI  (rawIg >> 2);
	result.Vd = MU_DirectV(rawVd >> 2);
	result.Id = MU_DirectI(rawId >> 2);

	return result;
}
// ----------------------------------------

void MU_LogScope(CombinedData MeasureSample)
{
	CombinedData tmp;
	static Int16U ScopeLogStep = 0, LocalCounter = 0;

	// Сброс локального счётчика в начале логгирования
	if (CONTROL_Values_Counter == 0)
		LocalCounter = 0;

	if (ScopeLogStep++ >= DataTable[REG_SCOPE_RATE])
	{
		ScopeLogStep = 0;

		CONTROL_Values_Vg[LocalCounter] = _IQint(MeasureSample.Vg);
		CONTROL_Values_Ig[LocalCounter] = _IQint(MeasureSample.Ig);
		CONTROL_Values_Vd[LocalCounter] = _IQint(MeasureSample.Vd);
		CONTROL_Values_Id[LocalCounter] = _IQint(MeasureSample.Id);

		tmp = REGULATOR_GetControl();
		CONTROL_Values_Ctrl_Vg[LocalCounter] = _IQint(tmp.Vg);
		CONTROL_Values_Ctrl_Ig[LocalCounter] = _IQint(tmp.Ig);
		CONTROL_Values_Ctrl_Vd[LocalCounter] = _IQint(tmp.Vd);
		CONTROL_Values_Ctrl_Id[LocalCounter] = _IQint(tmp.Id);

		tmp = REGULATOR_GetTarget();
		CONTROL_Values_Trgt_Vg[LocalCounter] = _IQint(tmp.Vg);
		CONTROL_Values_Trgt_Ig[LocalCounter] = _IQint(tmp.Ig);
		CONTROL_Values_Trgt_Vd[LocalCounter] = _IQint(tmp.Vd);
		CONTROL_Values_Trgt_Id[LocalCounter] = _IQint(tmp.Id);

		// Сохранение указателя на последний элемент
		DataTable[REG_EP_LAST_POINTER] = LocalCounter;

		++LocalCounter;
	}

	// Условие обновления глобального счётчика данных
	if (CONTROL_Values_Counter < VALUES_x_SIZE)
		CONTROL_Values_Counter = LocalCounter;

	// Сброс локального счётчика
	if (LocalCounter >= VALUES_x_SIZE)
		LocalCounter = 0;
}
// ----------------------------------------
