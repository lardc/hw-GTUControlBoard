// -----------------------------------------
// Universal regulator
// ----------------------------------------

// Header
#include "Regulator.h"
//
// Includes
#include "DeviceObjectDictionary.h"
#include "DataTable.h"
#include "Controller.h"
#include "stdlib.h"
#include "ZbBoard.h"
#include "ConvertUtils.h"
#include "IQmathLib.h"
#include "IQmathUtils.h"


// Types
typedef struct __RegulatorSettings
{
	Boolean Enabled;
	_iq TargetValue;
	_iq TargetValuePrev;
	_iq ErrorI;
	_iq Kp;
	_iq Ki;
	_iq Control;
	_iq ControlSat;
} RegulatorSettings, *pRegulatorSettings;


// Variables
RegulatorSettings RegulatorVg = {0}, RegulatorIg = {0}, RegulatorVd = {0}, RegulatorId = {0};


// Forward functions
void REGULATOR_CycleX(RegulatorSelector Selector, CombinedData MeasureSample);
void REGULATOR_Init(RegulatorSelector Selector);
void REGULATOR_InitX(pRegulatorSettings Regulator, _iq ControlSat, Int16U Register_Kp, Int16U Register_Ki);


// Functions
//
void REGULATOR_Cycle(CombinedData MeasureSample)
{
	// Регулятор напряжения цепи управления
	REGULATOR_CycleX(SelectVg, MeasureSample);

	// Регулятор тока цепи управления
	REGULATOR_CycleX(SelectIg, MeasureSample);

	// Регулятор напряжения силовой цепи
	REGULATOR_CycleX(SelectVd, MeasureSample);

	// Регулятор тока силовой цепи
	REGULATOR_CycleX(SelectId, MeasureSample);
}
// ----------------------------------------

void REGULATOR_CycleX(RegulatorSelector Selector, CombinedData MeasureSample)
{
	_iq SampleValue;
	pRegulatorSettings Regulator;

	switch (Selector)
	{
		case SelectVd:
			Regulator = &RegulatorVd;
			SampleValue = MeasureSample.Vd;
			break;

		case SelectId:
			Regulator = &RegulatorId;
			SampleValue = MeasureSample.Id;
			break;

		case SelectVg:
			Regulator = &RegulatorVg;
			SampleValue = MeasureSample.Vg;
			break;

		case SelectIg:
			Regulator = &RegulatorIg;
			SampleValue = MeasureSample.Ig;
			break;
	}

	if(Regulator->Enabled)
	{
		_iq ControlI = 0;
		_iq Error = Regulator->TargetValuePrev - SampleValue;

		// Расчёт интегральной ошибки
		if(Regulator->Ki)
		{
			Regulator->ErrorI += Error;

			// Проверка насыщения
			if(_IQabs(Regulator->ErrorI) > REGLTR_ERROR_I_SAT_H)
				Regulator->ErrorI = (Regulator->ErrorI > 0) ? REGLTR_ERROR_I_SAT_H : _IQmpy(_IQ(-1), REGLTR_ERROR_I_SAT_H);

			ControlI = _IQmpy(Regulator->ErrorI, Regulator->Ki);
		}
		else
		{
			Regulator->ErrorI = 0;
			ControlI = 0;
		}

		// Корректировка управления
		Regulator->Control = Regulator->TargetValue + _IQmpy(Error, Regulator->Kp) + ControlI;
		Regulator->TargetValuePrev = Regulator->TargetValue;

		// Проверка насыщения
		if(Regulator->Control < 0)
			Regulator->Control = 0;
		else if(Regulator->Control > Regulator->ControlSat)
			Regulator->Control = Regulator->ControlSat;

		// Применение значения
		REGULATOR_SetOutput(Selector, Regulator->Control);
	}
}
// ----------------------------------------

void REGULATOR_InitX(pRegulatorSettings Regulator, _iq ControlSat, Int16U Register_Kp, Int16U Register_Ki)
{
	Regulator->Enabled			= FALSE;
	Regulator->Kp				= _FPtoIQ2(DataTable[Register_Kp], 1000);
	Regulator->Ki 				= _FPtoIQ2(DataTable[Register_Ki], 1000);
	Regulator->ControlSat 		= ControlSat;

	Regulator->Control 			= 0;
	Regulator->ErrorI 			= 0;
	Regulator->TargetValue		= 0;
	Regulator->TargetValuePrev	= 0;
}
// ----------------------------------------

Boolean REGULATOR_IsIErrorSaturated(RegulatorSelector Selector)
{
	switch (Selector)
	{
		case SelectVd: return (_IQabs(RegulatorVd.ErrorI) == REGLTR_ERROR_I_SAT_H);
		case SelectId: return (_IQabs(RegulatorId.ErrorI) == REGLTR_ERROR_I_SAT_H);
		case SelectVg: return (_IQabs(RegulatorVg.ErrorI) == REGLTR_ERROR_I_SAT_H);
		case SelectIg: return (_IQabs(RegulatorIg.ErrorI) == REGLTR_ERROR_I_SAT_H);
	}

	return FALSE;
}
// ----------------------------------------

void REGULATOR_Enable(RegulatorSelector Selector, Boolean State)
{
	switch (Selector)
	{
		case SelectVd: RegulatorVd.Enabled = State; break;
		case SelectId: RegulatorId.Enabled = State; break;
		case SelectVg: RegulatorVg.Enabled = State; break;
		case SelectIg: RegulatorIg.Enabled = State; break;
	}
}
// ----------------------------------------

void REGULATOR_Update(RegulatorSelector Selector, _iq Value)
{
	switch (Selector)
	{
		case SelectVd: RegulatorVd.TargetValue = Value; break;
		case SelectId: RegulatorId.TargetValue = Value; break;
		case SelectVg: RegulatorVg.TargetValue = Value; break;
		case SelectIg: RegulatorIg.TargetValue = Value; break;
	}
}
// ----------------------------------------

void REGULATOR_SetOutput(RegulatorSelector Selector, _iq Value)
{
	switch (Selector)
	{
		case SelectVd:
			RegulatorVd.Control = Value;
			ZbDAC_DirectWriteVoltage(CU_DirectVtoDAC(Value));
			break;

		case SelectId:
			RegulatorId.Control = Value;
			ZbDAC_DirectWriteCurrent(CU_DirectItoDAC(Value));
			break;

		case SelectVg:
			RegulatorVg.Control = Value;
			ZbDAC_GateWriteVoltage(CU_GateVtoDAC(Value));
			break;

		case SelectIg:
			RegulatorIg.Control = Value;
			ZbDAC_GateWriteCurrent(CU_GateItoDAC(Value));
			break;
	}
}
// ----------------------------------------

void REGULATOR_Init(RegulatorSelector Selector)
{
	switch (Selector)
	{
		case SelectVd:
			REGULATOR_InitX(&RegulatorVd, REGLTR_VD_SAT, REG_PI_CTRL_VD_Kp, REG_PI_CTRL_VD_Ki);
			break;

		case SelectId:
			REGULATOR_InitX(&RegulatorId, REGLTR_ID_SAT, REG_PI_CTRL_ID_Kp, REG_PI_CTRL_ID_Ki);
			break;

		case SelectVg:
			REGULATOR_InitX(&RegulatorVg, REGLTR_VG_SAT, REG_PI_CTRL_VG_Kp, REG_PI_CTRL_VG_Ki);
			break;

		case SelectIg:
			REGULATOR_InitX(&RegulatorIg, REGLTR_IG_SAT, REG_PI_CTRL_IG_Kp, REG_PI_CTRL_IG_Ki);
			break;
	}
}
// ----------------------------------------

void REGULATOR_DisableAll()
{
	REGULATOR_Enable(SelectVg, FALSE);
	REGULATOR_Enable(SelectIg, FALSE);
	REGULATOR_Enable(SelectVd, FALSE);
	REGULATOR_Enable(SelectId, FALSE);
}
// ----------------------------------------

void REGULATOR_InitAll()
{
	REGULATOR_Init(SelectVg);
	REGULATOR_Init(SelectIg);
	REGULATOR_Init(SelectVd);
	REGULATOR_Init(SelectId);
}
// ----------------------------------------

void REGULATOR_ForceOutputsToZero()
{
	REGULATOR_SetOutput(SelectVg, 0);
	REGULATOR_SetOutput(SelectIg, 0);
	REGULATOR_SetOutput(SelectVd, 0);
	REGULATOR_SetOutput(SelectId, 0);
}
// ----------------------------------------

CombinedData REGULATOR_GetControl()
{
	CombinedData ret;

	ret.Vg = RegulatorVg.Control;
	ret.Ig = RegulatorIg.Control;
	ret.Vd = RegulatorVd.Control;
	ret.Id = RegulatorId.Control;

	return ret;
}
// ----------------------------------------

CombinedData REGULATOR_GetTarget()
{
	CombinedData ret;

	ret.Vg = RegulatorVg.TargetValue;
	ret.Ig = RegulatorIg.TargetValue;
	ret.Vd = RegulatorVd.TargetValue;
	ret.Id = RegulatorId.TargetValue;

	return ret;
}
// ----------------------------------------
