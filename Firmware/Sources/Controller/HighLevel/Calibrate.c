﻿// ----------------------------------------
// Logic of Gate calibration
// ----------------------------------------

// Header
#include "Calibrate.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Global.h"
#include "IQmathUtils.h"
#include "MeasureUtils.h"
#include "Regulator.h"
#include "Common.h"
#include "Constraints.h"


// Constants
//
#define CAL_SAMPLES_COUNT		100
//
#define I_SAT_VALUE				_IQI(1100.0f)
#define V_SAT_VALUE				_IQI(15000.0f)


// Types
//
typedef enum __CalibrateState
{
	CALIBRATE_STATE_AUX_RISE = 0,
	CALIBRATE_STATE_RISE,
	CALIBRATE_STATE_STAB_CHECK,
	CALIBRATE_STATE_COLLECT,
	CALIBRATE_STATE_FINISH_PREPARE,
	CALIBRATE_STATE_FINISH
} CalibrateState;


// Variables
//
static RegulatorSelector SavedSelector;
static CalibrateState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay;
static _iq Avg, AvgV, xLimit, xSetpoint, xChangeStep;

// Forward functions
//
void CALIBRATE_CacheVariables(RegulatorSelector Select);


// Functions
//
void CALIBRATE_Prepare(RegulatorSelector Select)
{
	// Кэширование переменных
	CALIBRATE_CacheVariables(Select);
	COMMON_PrepareStart();

	Delay = LogicSettings.StabCounter;

	switch (Select)
	{
		case SelectVd:
			REGULATOR_SetOutput(SelectId, I_SAT_VALUE);
			break;

		case SelectId:
			REGULATOR_SetOutput(SelectVd, V_SAT_VALUE);
			break;

		case SelectVg:
			REGULATOR_SetOutput(SelectIg, I_SAT_VALUE);
			break;

		case SelectIg:
			REGULATOR_SetOutput(SelectVg, V_SAT_VALUE);
			break;
	}

	State = CALIBRATE_STATE_AUX_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean CALIBRATE_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	_iq measure, measureV = 0;
	++LogicSettings.CycleCounter;

	switch (SavedSelector)
	{
		case SelectVd: measure = MeasureSample.Vd; break;
		case SelectId: measure = MeasureSample.Id; break;
		case SelectVg: measure = MeasureSample.Vg; break;
		case SelectIg:
			measure  = MeasureSample.Ig;
			measureV = MeasureSample.Vg;
			break;
	}

	switch (State)
	{
		// Ожидание готовности вспомогательного контура
		case CALIBRATE_STATE_AUX_RISE:
			{
				if (Delay == 0)
				{
					REGULATOR_Enable(SavedSelector, TRUE);
					State = CALIBRATE_STATE_RISE;
				}
				else
					--Delay;
			}
			break;

		// Нарастание основного сигнала
		case CALIBRATE_STATE_RISE:
			{
				xSetpoint += xChangeStep;
				if (xSetpoint > xLimit)
				{
					xSetpoint = xLimit;
					Delay = LogicSettings.StabCounter;

					State = CALIBRATE_STATE_STAB_CHECK;
				}
				REGULATOR_Update(SavedSelector, xSetpoint);
			}
			break;

		// Проверка корректности выхода на уставку
		case CALIBRATE_STATE_STAB_CHECK:
			{
				_iq ErrX = _IQdiv(_IQabs(measure - xSetpoint), xSetpoint);

				if ((Delay <= (LogicSettings.StabCounter / 2)) && (ErrX <= LogicSettings.AllowedError))
				{
					Delay = CAL_SAMPLES_COUNT;
					State = CALIBRATE_STATE_COLLECT;
				}
				else if (Delay == 0)
				{
					Int16U problem;
					switch (SavedSelector)
					{
						case SelectVd: problem = PROBLEM_VD_SET_ERR; break;
						case SelectId: problem = PROBLEM_ID_SET_ERR; break;
						case SelectVg: problem = PROBLEM_VG_SET_ERR; break;
						case SelectIg: problem = PROBLEM_IG_SET_ERR; break;
					}

					Codes->Problem = problem;
					State = CALIBRATE_STATE_FINISH_PREPARE;
				}
				else
					--Delay;
			}
			break;

		// Сбор значений
		case CALIBRATE_STATE_COLLECT:
			{
				if (Delay == 0)
				{
					DataTable[REG_RESULT_CAL] = _IQint(Avg);
					DataTable[REG_RESULT_CAL_FRAC] = _IQint(_IQmpy(_IQfrac(Avg), _IQ(1000)));

					if(SavedSelector == SelectIg && DataTable[REG_OLD_GTU_COMPATIBLE])
					{
						DataTable[REG_RESULT_CAL_V] = _IQint(AvgV);
						DataTable[REG_RESULT_CAL_V_FRAC] = _IQint(_IQmpy(_IQfrac(AvgV), _IQ(1000)));
					}

					State = CALIBRATE_STATE_FINISH_PREPARE;
				}
				else
				{
					Avg += _IQdiv(measure, _IQI(CAL_SAMPLES_COUNT));
					AvgV += _IQdiv(measureV, _IQI(CAL_SAMPLES_COUNT));
					--Delay;
				}
			}
			break;

		// Подготовка к завершению процесса
		case CALIBRATE_STATE_FINISH_PREPARE:
			{
				COMMON_PrepareFinish();
				Delay = LogicSettings.StabCounter;

				State = CALIBRATE_STATE_FINISH;
			}
			break;

		// Завершение процесса
		case CALIBRATE_STATE_FINISH:
			{
				if (Delay == 0)
				{
					COMMON_Finish();
					DataTable[REG_PROCESS_COUNTER] = LogicSettings.CycleCounter;
					return TRUE;
				}
				else
					--Delay;
			}
			break;
	}

	return FALSE;
}
// ----------------------------------------

void CALIBRATE_CacheVariables(RegulatorSelector Select)
{
	COMMON_CacheCalibrationVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);

	switch (Select)
	{
		case SelectVd:
			xLimit = Vd.Limit;
			xChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_CAL_V_RISE_RATE]));
			break;

		case SelectId:
			xLimit = Id.Limit;
			xChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_CAL_I_RISE_RATE]));
			break;

		case SelectVg:
			xLimit = Vg.Limit;
			xChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_CAL_V_RISE_RATE]));
			break;

		case SelectIg:
			xLimit = Ig.Limit;
			xChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_CAL_I_RISE_RATE]));
			break;
	}

	if(DataTable[REG_OLD_GTU_COMPATIBLE] && (Select == SelectId || Select == SelectIg))
		xLimit = _IQI(DataTable[REG_CAL_CURRENT]);

	xSetpoint = 0;
	Avg = AvgV = 0;
	SavedSelector = Select;
}
// ----------------------------------------
