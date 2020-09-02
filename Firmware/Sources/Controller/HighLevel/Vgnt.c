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
	GATE_STATE_V_CONFIRM,
	GATE_STATE_FINISH_PREPARE,
	GATE_STATE_FINISH
} VGNTState;

// Variables
//
static VGNTState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay, ConfirmationTimeCounter;
static Boolean ConfirmationMode;

// Forward functions
//
void VGNT_CacheVariables();

// Functions
//
void VGNT_Prepare()
{
	// Кэширование переменных
	VGNT_CacheVariables();
	COMMON_PrepareStart();
	
	// Активация регуляторов
	REGULATOR_Enable(SelectVg, TRUE);
	REGULATOR_Enable(SelectIg, TRUE);
	
	// Выставление начальных значений
	REGULATOR_Update(SelectVg, 0);
	REGULATOR_Update(SelectIg, Ig.Limit);
	
	// Включение подстройки напряжения питания для снижения нагрева ОУ
	ZbGPIO_GatePSTune(TRUE);
	
	State = GATE_STATE_V_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean VGNT_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	// Detect Vg sensing disconnection
	if(State == GATE_STATE_V_RISE || State == GATE_STATE_V_CONFIRM)
	{
		_iq ErrIg = _IQdiv(_IQabs(MeasureSample.Ig - Ig.Limit), Ig.Limit);

		if(ErrIg > LogicSettings.AllowedError && REGULATOR_IsIErrorSaturated(SelectVg))
		{
			Codes->Problem = PROBLEM_DUT_NO_VG_SENSING;
			State = GATE_STATE_FINISH_PREPARE;
		}
	}

	++LogicSettings.CycleCounter;
	
	switch (State)
	{
		// Процесс нарастания напряжения Vg
		case GATE_STATE_V_RISE:
			{
				Vg.Setpoint += Vg.ChangeStep;
				if(Vg.Setpoint > Vg.Limit)
					Vg.Setpoint = Vg.Limit;
				
				if(ZbGPIO_ExtSyncTrig())
				{
					if(ConfirmationMode)
						Codes->Problem = PROBLEM_VGNT_CONF_TRIG;
					else
					{
						DataTable[REG_RESULT_VGNT] = _IQint(MeasureSample.Vg);
						DataTable[REG_RESULT_IGNT] = _IQint(MeasureSample.Ig);
					}
					State = GATE_STATE_FINISH_PREPARE;
				}
				else if(Vg.Setpoint == Vg.Limit)
				{
					if(ConfirmationMode)
					{
						Delay = ConfirmationTimeCounter;
						State = GATE_STATE_V_CONFIRM;
					}
					else
					{
						Codes->Problem = PROBLEM_DUT_NO_TRIG;
						State = GATE_STATE_FINISH_PREPARE;
					}
				}

				REGULATOR_Update(SelectVg, Vg.Setpoint);
			}
			break;
			
		// Процесс удержания напряжения Vg
		case GATE_STATE_V_CONFIRM:
			{
				if(Delay == 0)
				{
					DataTable[REG_RESULT_VGNT] = _IQint(MeasureSample.Vg);
					DataTable[REG_RESULT_IGNT] = _IQint(MeasureSample.Ig);
					State = GATE_STATE_FINISH_PREPARE;
				}
				else
				{
					--Delay;
					if(ZbGPIO_ExtSyncTrig())
					{
						Codes->Problem = PROBLEM_VGNT_CONF_TRIG;
						State = GATE_STATE_FINISH_PREPARE;
					}
				}
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

void VGNT_CacheVariables()
{
	_iq VRate_mV_s;
	
	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);
	
	ConfirmationMode = DataTable[REG_VGNT_CONF_MODE];
	ConfirmationTimeCounter = 1000L * DataTable[REG_VGNT_CONF_TIME] / TIMER0_PERIOD;
	VRate_mV_s = _FPtoIQ2(DataTable[ConfirmationMode ? REG_VGNT_CONF_VG_RATE : REG_VGNT_VG_RATE], 1000);
	Vg.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), VRate_mV_s);
}
// ----------------------------------------
