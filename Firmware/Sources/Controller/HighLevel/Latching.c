// -----------------------------------------
// Logic of latching current testing process
// ----------------------------------------

// Header
#include "Latching.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Controller.h"
#include "IQmathUtils.h"
#include "MeasureUtils.h"
#include "Regulator.h"
#include "Common.h"


// Types
//
typedef enum __LatchingState
{
	LATCHING_STATE_VD_RISE = 0,
	LATCHING_STATE_VD_STAB,
	LATCHING_STATE_VD_CHECK,
	LATCHING_STATE_GATE_ON_STAB,
	LATCHING_STATE_GATE_OFF_STAB,
	LATCHING_STATE_ID_CHECK,
	LATCHING_STATE_DUT_CLOSE,
	LATCHING_STATE_FINISH_PREPARE,
	LATCHING_STATE_FINISH
} LatchingState;


// Variables
//
static LatchingState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay;
static _iq IdTestCurrent, IdSmallStep;


// Forward functions
//
void LATCHING_CacheVariables();


// Functions
//
void LATCHING_Prepare()
{
	// ����������� ����������
	LATCHING_CacheVariables();
	COMMON_PrepareStart();

	// ��������� �����������
	REGULATOR_Enable(SelectVd, TRUE);
	REGULATOR_Enable(SelectVg, TRUE);
	REGULATOR_Enable(SelectId, TRUE);
	REGULATOR_Enable(SelectIg, TRUE);

	// ����������� ��������� ��������
	REGULATOR_Update(SelectVd, 0);
	REGULATOR_Update(SelectVg, Vg.Limit);
	REGULATOR_Update(SelectId, IdTestCurrent);
	REGULATOR_Update(SelectIg, 0);

	State = LATCHING_STATE_VD_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean LATCHING_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	++LogicSettings.CycleCounter;

	switch (State)
	{
		// ����� �� ���������� Vd
		case LATCHING_STATE_VD_RISE:
			{
				Vd.Setpoint += Vd.ChangeStep;
				if (Vd.Setpoint > Vd.Limit)
				{
					Vd.Setpoint = Vd.Limit;
					Delay = LogicSettings.StabCounter;

					State = LATCHING_STATE_VD_STAB;
				}
				REGULATOR_Update(SelectVd, Vd.Setpoint);
			}
			break;

		// �������� �� ������������ ������ Vd
		case LATCHING_STATE_VD_STAB:
			{
				if (Delay == 0)
					State = LATCHING_STATE_VD_CHECK;
				else
					--Delay;
			}
			break;

		// �������� ������������ ������ �� ������� Vd
		case LATCHING_STATE_VD_CHECK:
			{
				_iq ErrVd = _IQdiv(_IQabs(MeasureSample.Vd - Vd.Setpoint), Vd.Setpoint);

				if (MeasureSample.Id > LogicSettings.IdLeak)
				{
					Codes->Problem = PROBLEM_ID_LEAK;
					State = LATCHING_STATE_FINISH_PREPARE;
				}
				else if (ErrVd > LogicSettings.AllowedError)
				{
					Codes->Problem = PROBLEM_VD_SET_ERR;
					State = LATCHING_STATE_FINISH_PREPARE;
				}
				else
				{
					// ��������� ������� - ����������� ���� Ig
					REGULATOR_Update(SelectIg, Ig.Limit);

					Delay = LogicSettings.StabCounter;
					State = LATCHING_STATE_GATE_ON_STAB;
				}
			}
			break;

			// ������������ ������ Vg ����� ���������
			case LATCHING_STATE_GATE_ON_STAB:
				{
					if (Delay == 0)
					{
						// �������� ���� � ������� ����
						_iq ErrId = _IQdiv(_IQabs(MeasureSample.Id - IdTestCurrent), IdTestCurrent);

						if (ErrId < LogicSettings.AllowedError)
						{
							REGULATOR_Update(SelectIg, 0);

							Delay = LogicSettings.StabCounter;
							State = LATCHING_STATE_GATE_OFF_STAB;
						}
						else
						{
							Codes->Problem = PROBLEM_DUT_NO_TRIG;
							State = LATCHING_STATE_FINISH_PREPARE;
						}
					}
					else
						--Delay;
				}
				break;

			// ������������ ������ Ig ����� ����������
			case LATCHING_STATE_GATE_OFF_STAB:
				{
					if (Delay == 0)
						State = LATCHING_STATE_ID_CHECK;
					else
						--Delay;
				}
				break;

			// �������� �������� ���� �� ������ ����� ����������
			case LATCHING_STATE_ID_CHECK:
				{
					if (MeasureSample.Id < LogicSettings.IdLeak)
					{
						// ������ ��������
						IdTestCurrent += Id.ChangeStep;

						// ���� ��������� ����� ���� - ������
						if (IdTestCurrent >= Id.Limit)
						{
							Codes->Problem = PROBLEM_DUT_NO_LATCHING;
							State = LATCHING_STATE_FINISH_PREPARE;
						}
						else
						{
							REGULATOR_Update(SelectId, IdTestCurrent);
							Delay = LogicSettings.StabCounter;
							State = LATCHING_STATE_VD_STAB;
						}
					}
					else
					{
						// ������ ������

						// ��������� ������ ����
						if (Id.ChangeStep != IdSmallStep)
						{
							// ������ �� ��� ����� �� ����
							IdTestCurrent -= Id.ChangeStep;

							// ���������� ����
							Id.ChangeStep = IdSmallStep;

							// ��������� �������
							REGULATOR_Update(SelectId, 0);
							Delay = LogicSettings.StabCounter;
							State = LATCHING_STATE_DUT_CLOSE;
						}
						else
						{
							// ���������� ���������
							DataTable[REG_RESULT_IL] = _IQint(MeasureSample.Id);
							State = LATCHING_STATE_FINISH_PREPARE;
						}
					}
				}
				break;

			// ����� ��������� �������
			case LATCHING_STATE_DUT_CLOSE:
				{
					if (Delay == 0)
					{
						REGULATOR_Update(SelectId, IdTestCurrent);
						Delay = LogicSettings.StabCounter;
						State = LATCHING_STATE_VD_STAB;
					}
					else
						--Delay;
				}
				break;

			// ���������� ��������
			case LATCHING_STATE_FINISH_PREPARE:
				{
					COMMON_PrepareFinish();

					Delay = LogicSettings.StabCounter;
					State = LATCHING_STATE_FINISH;
				}
				break;

			// ���������� ��������
			case LATCHING_STATE_FINISH:
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

void LATCHING_CacheVariables()
{
	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);

	LogicSettings.StabCounter = 1000L * DataTable[REG_LATCH_STAB_TIME] / TIMER0_PERIOD;
	IdTestCurrent = _IQI(DataTable[REG_LATCH_START_CURRENT]);
	Id.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_LATCH_RATE_LARGE]));
	IdSmallStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_LATCH_RATE_SMALL]));
}
// ----------------------------------------
