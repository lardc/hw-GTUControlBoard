// -----------------------------------------
// Logic of Gate testing
// ----------------------------------------

// Header
#include "Gate.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "ZbGPIO.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Global.h"
#include "IQmathUtils.h"
#include "MeasureUtils.h"
#include "Regulator.h"
#include "Common.h"


// Types
//
typedef enum __VGNTState
{
	GATE_STATE_V_RISE = 0,
	GATE_STATE_FINISH_PREPARE,
	GATE_STATE_FINISH
} VGNTState;


// Variables
//
static VGNTState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay;


// Forward functions
//
void VGNT_CacheVariables();


// Functions
//
void VGNT_Prepare()
{
	// ����������� ����������
	VGNT_CacheVariables();
	COMMON_PrepareStart();

	// ��������� �����������
	REGULATOR_Enable(SelectVg, TRUE);
	REGULATOR_Enable(SelectIg, TRUE);

	// ����������� ��������� ��������
	REGULATOR_Update(SelectVg, 0);
	REGULATOR_Update(SelectIg, Ig.Limit);

	// ��������� ���������� ���������� ������� ��� �������� ������� ��
	ZbGPIO_GatePSTune(TRUE);

	State = GATE_STATE_V_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean VGNT_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	++LogicSettings.CycleCounter;

	switch (State)
	{
		// ������� ���������� ���������� Vg
		case GATE_STATE_V_RISE:
			{
				_iq ErrIg = _IQdiv(_IQabs(MeasureSample.Ig - Ig.Limit), Ig.Limit);

				if(!ZbGPIO_ExtSyncReadState())
				{
					DataTable[REG_RESULT_VGNT] = _IQint(Vg.Setpoint - Vg.ChangeStep);
					State = GATE_STATE_FINISH_PREPARE;
				}
				else
				{
					Vg.Setpoint += Vg.ChangeStep;
					if (Vg.Setpoint > Vg.Limit)
						Vg.Setpoint = Vg.Limit;

					if((Vg.Setpoint == Vg.Limit) || (MeasureSample.Ig >= Ig.Limit))
					{
						Codes->Problem = PROBLEM_DUT_NO_TRIG;
						Delay = LogicSettings.StabCounter;
						State = GATE_STATE_FINISH_PREPARE;
					}

					REGULATOR_Update(SelectVg, Vg.Setpoint);
				}
			}
			break;

		// ���������� � ���������� ��������
		case GATE_STATE_FINISH_PREPARE:
			{
				COMMON_PrepareFinish();
				Delay = LogicSettings.StabCounter;

				State = GATE_STATE_FINISH;
			}
			break;

		// ���������� ��������
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

void VGNT_CacheVariables()
{
	_iq VRate_mV_s;

	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);

	VRate_mV_s = _FPtoIQ2(DataTable[REG_VGNT_VG_RATE], 1000);
	Vg.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), VRate_mV_s);
}
// ----------------------------------------
