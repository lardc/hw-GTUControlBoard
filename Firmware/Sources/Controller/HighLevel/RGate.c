// ----------------------------------------
// Logic of Gate resistance testing
// ----------------------------------------

// Header
#include "RGate.h"
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


// Definitions
//
#define RG_SAMPLES_COUNT		16


// Types
//
typedef enum __RGateState
{
	RGATE_STATE_VG_RISE = 0,
	RGATE_STATE_IG_RISE,
	RGATE_STATE_IG_STAB,
	RGATE_STATE_IG_CHECK,
	RGATE_STATE_COLLECT,
	RGATE_STATE_FINISH_PREPARE,
	RGATE_STATE_FINISH
} RGateState;


// Variables
//
static RGateState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay;
static _iq RgAvg;


// Forward functions
//
void RGATE_CacheVariables();


// Functions
//
void RGATE_Prepare()
{
	// Кэширование переменных
	RGATE_CacheVariables();
	COMMON_PrepareStart();

	// Активация регуляторов
	REGULATOR_Enable(SelectVg, TRUE);
	REGULATOR_Enable(SelectIg, TRUE);

	// Выставление начальных значений
	REGULATOR_Update(SelectVg, Vg.Limit);
	REGULATOR_Update(SelectIg, 0);

	Delay = LogicSettings.StabCounter;

	State = RGATE_STATE_VG_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean RGATE_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	++LogicSettings.CycleCounter;

	switch(State)
	{
		// Ожидание нарастания напряжения контура Vg
		case RGATE_STATE_VG_RISE:
			{
				if (Delay == 0)
				{
					State = RGATE_STATE_IG_RISE;
				}
				else
					--Delay;
			}
			break;

		// Нарастание тока Ig
		case RGATE_STATE_IG_RISE:
			{
				Ig.Setpoint += Ig.ChangeStep;
				if (Ig.Setpoint > Ig.Limit)
				{
					Ig.Setpoint = Ig.Limit;
					Delay = LogicSettings.StabCounter;

					State = RGATE_STATE_IG_STAB;
				}
				REGULATOR_Update(SelectIg, Ig.Setpoint);
			}
			break;

		// Задержка на стабилизацию выхода Ig
		case RGATE_STATE_IG_STAB:
			{
				if (Delay == 0)
				{
					// Проверка обрыва потенциальной линии управления Vg
					if(MeasureSample.Vg < LogicSettings.VgMinInput && REGULATOR_IsIErrorSaturated(SelectVg))
					{
						Codes->Problem = PROBLEM_DUT_NO_VG_SENSING;
						State = RGATE_STATE_FINISH_PREPARE;
					}
					else
						State = RGATE_STATE_IG_CHECK;
				}
				else
					--Delay;
			}
			break;

		// Проверка корректности выхода на уставку Ig
		case RGATE_STATE_IG_CHECK:
			{
				_iq ErrIg = _IQdiv(_IQabs(MeasureSample.Ig - Ig.Setpoint), Ig.Setpoint);

				if (ErrIg > LogicSettings.AllowedError)
				{
					Codes->Problem = PROBLEM_IG_SET_ERR;
					State = RGATE_STATE_FINISH_PREPARE;
				}
				else
				{
					Delay = RG_SAMPLES_COUNT;
					State = RGATE_STATE_COLLECT;
				}

			}
			break;

		// Сбор значений сопротивления
		case RGATE_STATE_COLLECT:
			{
				if (Delay == 0)
				{
					DataTable[REG_RESULT_RG] = _IQint(_IQdiv(_IQmpy(RgAvg, _IQ(10.0f)), _IQI(RG_SAMPLES_COUNT)));
					State = RGATE_STATE_FINISH_PREPARE;
				}
				else
				{
					RgAvg += _IQdiv(MeasureSample.Vg, MeasureSample.Ig);
					--Delay;
				}
			}
			break;

		// Подготовка к завершению процесса
		case RGATE_STATE_FINISH_PREPARE:
			{
				COMMON_PrepareFinish();
				Delay = LogicSettings.StabCounter;

				State = RGATE_STATE_FINISH;
			}
			break;

		// Завершение процесса
		case RGATE_STATE_FINISH:
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

void RGATE_CacheVariables()
{
	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);

	RgAvg = 0;
	Ig.Limit = _IQI(DataTable[REG_RG_CURRENT]);
	Ig.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_RG_CURRENT_RATE]));
}
// ----------------------------------------
