// -----------------------------------------
// Logic of Gate testing
// ----------------------------------------

// Header
#include "Gate.h"
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


// Types
//
typedef enum __GateState
{
	GATE_STATE_V_RISE = 0,
	GATE_STATE_V_STAB,
	GATE_STATE_IL_CHECK,
	GATE_STATE_IG_RISE,
	GATE_STATE_MEASURE_STAB,
	GATE_STATE_FINISH_PREPARE,
	GATE_STATE_FINISH
} GateState;


// Variables
//
static GateState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay;
static Boolean KeepAnodeCurrent;


// Forward functions
//
void GATE_CacheVariables();


// Functions
//
void GATE_Prepare()
{
	// Кэширование переменных
	GATE_CacheVariables();
	COMMON_PrepareStart();

	// Активация регуляторов
	REGULATOR_Enable(SelectVd, TRUE);
	REGULATOR_Enable(SelectVg, TRUE);
	REGULATOR_Enable(SelectId, TRUE);
	REGULATOR_Enable(SelectIg, TRUE);

	// Выставление начальных значений
	REGULATOR_Update(SelectVd, 0);
	REGULATOR_Update(SelectVg, 0);
	REGULATOR_Update(SelectId, Id.Limit);
	REGULATOR_Update(SelectIg, 0);

	State = GATE_STATE_V_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean GATE_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	++LogicSettings.CycleCounter;

	switch (State)
	{
		// Выход на напряжение Vd, Vg
		case GATE_STATE_V_RISE:
			{
				Vd.Setpoint += Vd.ChangeStep;
				if (Vd.Setpoint > Vd.Limit)
					Vd.Setpoint = Vd.Limit;

				Vg.Setpoint += Vg.ChangeStep;
				if (Vg.Setpoint > Vg.Limit)
					Vg.Setpoint = Vg.Limit;

				if((Vd.Setpoint == Vd.Limit) && (Vg.Setpoint == Vg.Limit))
				{
					Delay = LogicSettings.StabCounter;
					State = GATE_STATE_V_STAB;
				}

				REGULATOR_Update(SelectVd, Vd.Setpoint);
				REGULATOR_Update(SelectVg, Vg.Setpoint);
			}
			break;

		// Задержка на стабилизацию выхода Vd, Vg
		case GATE_STATE_V_STAB:
			{
				if (Delay == 0)
					State = GATE_STATE_IL_CHECK;
				else
					--Delay;
			}
			break;

		// Проверка отсутствия тока утечки прямой цепи
		case GATE_STATE_IL_CHECK:
			{
				if (MeasureSample.Id > LogicSettings.IdLeak)
				{
					Codes->Problem = PROBLEM_ID_LEAK;
					State = GATE_STATE_FINISH_PREPARE;
				}
				else
				{
					Delay = LogicSettings.StabCounter;
					State = GATE_STATE_IG_RISE;
				}
			}
			break;

		// Запуск формирования тока управления
		case GATE_STATE_IG_RISE:
			{
				_iq ErrId = _IQdiv(_IQabs(MeasureSample.Id - Id.Limit), Id.Limit);
				_iq ErrIg = _IQdiv(_IQabs(MeasureSample.Ig - Ig.Limit), Ig.Limit);

				// Проверка условия отпирания прибора
				if (ErrId < LogicSettings.AllowedError)
				{
					if (!KeepAnodeCurrent)
					{
						// Снятие анодного тока
						REGULATOR_Enable(SelectId, FALSE);
						REGULATOR_SetOutput(SelectId, 0);
					}

					Delay = LogicSettings.StabCounter;
					State = GATE_STATE_MEASURE_STAB;
				}

				// Нарастание тока управления
				if (Ig.Setpoint != Ig.Limit)
				{
					Ig.Setpoint += Ig.ChangeStep;
					if (Ig.Setpoint > Ig.Limit)
						Ig.Setpoint = Ig.Limit;

					REGULATOR_Update(SelectIg, Ig.Setpoint);
				}
				else
				{
					// Прибор не сработал после выхода на Ig
					if (Delay == 0)
					{
						Codes->Problem = PROBLEM_DUT_NO_TRIG;
						State = GATE_STATE_FINISH_PREPARE;
					}
					else
						--Delay;
				}
			}
			break;

		// Стабилизация и измерение
		case GATE_STATE_MEASURE_STAB:
			{
				if (Delay == 0)
				{
					// Снятие анодного тока
					REGULATOR_Enable(SelectId, FALSE);
					REGULATOR_SetOutput(SelectId, 0);

					// Сохранение результата
					DataTable[REG_RESULT_IGT] = _IQint(MeasureSample.Ig);
					DataTable[REG_RESULT_VGT] = _IQint(MeasureSample.Vg);


					State = GATE_STATE_FINISH_PREPARE;
				}
				else
					--Delay;
			}
			break;

		// Подготовка к завершению процесса
		case GATE_STATE_FINISH_PREPARE:
			{
				COMMON_PrepareFinish();
				Delay = LogicSettings.StabCounter;

				State = GATE_STATE_FINISH;
			}
			break;

		// Завершение процесса
		case GATE_STATE_FINISH:
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

void GATE_CacheVariables()
{
	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);

	Vg.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_GATE_VG_RATE]));
	Ig.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_RG_CURRENT_RATE]));
	KeepAnodeCurrent = DataTable[REG_GATE_KEEP_ANODE_CURR];
}
// ----------------------------------------
