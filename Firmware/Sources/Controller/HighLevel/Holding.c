// ----------------------------------------
// Controller of Holding current testing
// ----------------------------------------

// Header
#include "Holding.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "IQmathUtils.h"
#include "MeasureUtils.h"
#include "Regulator.h"
#include "Common.h"

// Types
//
typedef enum __HoldingState
{
	HOLDING_STATE_VD_RISE = 0,
	HOLDING_STATE_VD_STAB,
	HOLDING_STATE_VD_CHECK,
	HOLDING_STATE_GATE_STAB,
	HOLDING_STATE_TRIG_CHECK,
	HOLDING_STATE_HOLD_CHECK,
	HOLDING_STATE_ID_FALL,
	HOLDING_STATE_FINISH_PREPARE,
	HOLDING_STATE_FINISH
} HoldingState;

// Variables
//
static HoldingState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay, TrigWaitGOST;
static _iq IdMinCurrent, VdCloseLevel;
static Boolean CombinedWithSL;

// Кольцевой буфер
static _iq PrevSampleId[IH_RING_BUFFER_SIZE];
static Int16U PrevSampleCounter, BackShiftSteps;
static Boolean RingBufferFull;

// Forward functions
//
void HOLDING_CacheVariables();

// Functions
//
void HOLDING_Prepare()
{
	// Кэширование переменных
	HOLDING_CacheVariables();
	COMMON_PrepareStart();

	// Активация регуляторов
	REGULATOR_Enable(SelectVd, TRUE);
	REGULATOR_Enable(SelectId, TRUE);
	REGULATOR_Enable(SelectVg, !CombinedWithSL);
	REGULATOR_Enable(SelectIg, !CombinedWithSL);

	// Выставление начальных значений
	REGULATOR_Update(SelectVd, 0);
	REGULATOR_Update(SelectId, Id.Limit);
	REGULATOR_Update(SelectIg, 0);
	REGULATOR_Update(SelectVg, CombinedWithSL ? 0 : Vg.Limit);

	State = HOLDING_STATE_VD_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean HOLDING_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	++LogicSettings.CycleCounter;

	switch(State)
	{
		// Выход на напряжение Vd
		case HOLDING_STATE_VD_RISE:
			{
				Vd.Setpoint += Vd.ChangeStep;
				if(Vd.Setpoint > Vd.Limit)
				{
					Vd.Setpoint = Vd.Limit;
					Delay = LogicSettings.StabCounter;

					State = HOLDING_STATE_VD_STAB;
				}
				REGULATOR_Update(SelectVd, Vd.Setpoint);
			}
			break;

		// Задержка на стабилизацию выхода Vd
		case HOLDING_STATE_VD_STAB:
			{
				if(Delay == 0)
					State = HOLDING_STATE_VD_CHECK;
				else
					--Delay;
			}
			break;

		// Проверка корректности выхода на уставку Vd
		case HOLDING_STATE_VD_CHECK:
			{
				_iq ErrVd = _IQdiv(_IQabs(MeasureSample.Vd - Vd.Setpoint), Vd.Setpoint);

				if(MeasureSample.Id > LogicSettings.IdLeak)
				{
					Codes->Problem = PROBLEM_ID_LEAK;
					State = HOLDING_STATE_FINISH_PREPARE;
				}
				else if(ErrVd > LogicSettings.AllowedError)
				{
					Codes->Problem = PROBLEM_VD_SET_ERR;
					State = HOLDING_STATE_FINISH_PREPARE;
				}
				else if(CombinedWithSL)
				{
					// Готовность к отпиранию прибора
					Delay = TrigWaitGOST;
					State = HOLDING_STATE_TRIG_CHECK;
				}
				else
				{
					// Отпирание прибора - выставление напряжения Ig
					REGULATOR_Update(SelectIg, Ig.Limit);
					Delay = LogicSettings.StabCounter;
					State = HOLDING_STATE_GATE_STAB;
				}
			}
			break;

		// Стабилизация выхода Ig
		case HOLDING_STATE_GATE_STAB:
			{
				if(Delay == 0)
				{
					// Проверка обрыва потенциальной линии управления Vg
					if(MeasureSample.Vg < LogicSettings.VgMinInput && REGULATOR_IsIErrorSaturated(SelectVg))
					{
						Codes->Problem = PROBLEM_DUT_NO_VG_SENSING;
						State = HOLDING_STATE_FINISH_PREPARE;
					}
					else
						State = HOLDING_STATE_TRIG_CHECK;
				}
				else
					--Delay;
			}
			break;

		// Проверка отпирания прибора
		case HOLDING_STATE_TRIG_CHECK:
			{
				_iq ErrId = _IQdiv(_IQabs(MeasureSample.Id - Id.Limit), Id.Limit);

				// Проверка условия отпирания прибора
				if(ErrId < LogicSettings.AllowedError)
				{
					// Снятие сигнала управления
					REGULATOR_Enable(SelectIg, FALSE);
					REGULATOR_Enable(SelectVg, FALSE);
					REGULATOR_SetOutput(SelectIg, 0);
					REGULATOR_SetOutput(SelectVg, 0);

					Id.Setpoint = Id.Limit;
					Delay = LogicSettings.StabCounter;
					State = HOLDING_STATE_HOLD_CHECK;
				}
				else if(Delay == 0)
				{
					Codes->Problem = PROBLEM_DUT_NO_TRIG;
					State = HOLDING_STATE_FINISH_PREPARE;
				}
				else
					--Delay;
			}
			break;

		case HOLDING_STATE_HOLD_CHECK:
			{
				if(Delay == 0)
				{
					if(MeasureSample.Id < LogicSettings.IdLeak)
					{
						Codes->Problem = PROBLEM_DUT_NO_LATCHING;
						State = HOLDING_STATE_FINISH_PREPARE;
					}
					else
					{
						Delay = LogicSettings.StabCounter;
						State = HOLDING_STATE_ID_FALL;
					}
				}
				else
					--Delay;
			}
			break;

		// Снижение анодного тока Id
		case HOLDING_STATE_ID_FALL:
			{
				// Если прибор закрылся - поиск предыдущего значения в кольцевом буфере
				if(MeasureSample.Id < LogicSettings.IdLeak || MeasureSample.Vd > VdCloseLevel)
				{
					// Поиск значения в кольцевом буфере
					_iq res;
					if(BackShiftSteps <= PrevSampleCounter)
						res = PrevSampleId[PrevSampleCounter - BackShiftSteps];
					else if(RingBufferFull)
						res = PrevSampleId[IH_RING_BUFFER_SIZE + PrevSampleCounter - BackShiftSteps];
					else
						res = PrevSampleId[0];

					DataTable[REG_RESULT_IH] = _IQint(res);
					DataTable[REG_RESULT_IH_UA] = _IQint(_IQmpy(_IQfrac(res), _IQ(1000)));

					State = HOLDING_STATE_FINISH_PREPARE;
				}
				else
				{
					PrevSampleId[PrevSampleCounter++] = MeasureSample.Id;
					if(PrevSampleCounter >= IH_RING_BUFFER_SIZE)
					{
						PrevSampleCounter = 0;
						RingBufferFull = TRUE;
					}

					// Снижение прямого тока
					if(Id.Setpoint != IdMinCurrent)
					{
						Id.Setpoint -= Id.ChangeStep;
						if(Id.Setpoint < IdMinCurrent)
							Id.Setpoint = IdMinCurrent;

						REGULATOR_Update(SelectId, Id.Setpoint);
					}
					else
					{
						// Прибор не закрылся после выхода на минимум Id
						if(Delay == 0)
						{
							Codes->Problem = PROBLEM_DUT_NO_CLOSE;
							State = HOLDING_STATE_FINISH_PREPARE;
						}
						else
							--Delay;
					}
				}
			}
			break;

		// Подготовка к завершению процесса
		case HOLDING_STATE_FINISH_PREPARE:
			{
				COMMON_PrepareFinish();

				Delay = LogicSettings.StabCounter;
				State = HOLDING_STATE_FINISH;
			}
			break;

		// Завершение процесса
		case HOLDING_STATE_FINISH:
			{
				if(Delay == 0)
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

void HOLDING_CacheVariables()
{
	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);

	Id.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_HOLD_CURRENT_FALL_RATE]));
	VdCloseLevel = _IQI(DataTable[REG_HOLD_VCLOSE_LEVEL]);
	IdMinCurrent = _IQI(DataTable[REG_HOLD_END_CURRENT]);
	TrigWaitGOST = 1000L * DataTable[REG_HOLD_TRIG_WAIT_GOST] / TIMER0_PERIOD;

	CombinedWithSL = DataTable[REG_HOLD_WITH_SL];

	PrevSampleCounter = 0;
	RingBufferFull = FALSE;
	BackShiftSteps = DataTable[REG_HOLD_BACK_SHIFT];
}
// ----------------------------------------
