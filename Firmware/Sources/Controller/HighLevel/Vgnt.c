// ----------------------------------------
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
typedef struct __SamplePoint
{
	Int16U Voltage;
	Int16U Current;
} SamplePoint, *pSamplePoint;

// Variables
//
static VGNTState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static Int16U Delay, ConfirmationTimeCounter, ErrZoneMonitoring, ResultPointOffset;
static Boolean ConfirmationMode;
// Ring buffer
static SamplePoint RingBuffer[VGNT_RING_BUFFER_SIZE];
static Boolean BufferIsFull;
static Int16S BufferPointer;

// Forward functions
//
void VGNT_CacheVariables();
void VGNT_SaveToRingBuffer(_iq Voltage, _iq Current);
SamplePoint VGNT_RingBufferRead(Int16U Offset);

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
		if(MeasureSample.Vg < LogicSettings.VgMinInput && REGULATOR_IsIErrorSaturated(SelectVg) &&
			LogicSettings.CycleCounter < ErrZoneMonitoring)
		{
			Codes->Problem = PROBLEM_DUT_NO_VG_SENSING;
			State = GATE_STATE_FINISH_PREPARE;
		}
	}

	VGNT_SaveToRingBuffer(MeasureSample.Vg, MeasureSample.Ig);
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
						SamplePoint Sample = VGNT_RingBufferRead(ResultPointOffset);
						DataTable[REG_RESULT_VGNT] = Sample.Voltage;
						DataTable[REG_RESULT_IGNT] = Sample.Current;
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
					SamplePoint Sample = VGNT_RingBufferRead(0);
					DataTable[REG_RESULT_VGNT] = Sample.Voltage;
					DataTable[REG_RESULT_IGNT] = Sample.Current;
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

void VGNT_SaveToRingBuffer(_iq Voltage, _iq Current)
{
	RingBuffer[BufferPointer].Voltage = _IQint(Voltage);
	RingBuffer[BufferPointer].Current = _IQint(Current);

	BufferPointer++;
	if(BufferPointer >= VGNT_RING_BUFFER_SIZE)
	{
		BufferPointer = 0;
		BufferIsFull = TRUE;
	}
}
// ----------------------------------------

SamplePoint VGNT_RingBufferRead(Int16U Offset)
{
	Int16S index, LastElementPointer = BufferPointer - Offset - 1;
	if(LastElementPointer >= 0)
		index = LastElementPointer;
	else
		index = BufferIsFull ? (LastElementPointer + VGNT_RING_BUFFER_SIZE) : 0;

	return RingBuffer[index];
}
// ----------------------------------------

void VGNT_CacheVariables()
{
	_iq VRate_mV_s;
	
	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);
	
	ConfirmationMode = DataTable[REG_VGNT_CONF_MODE];
	ConfirmationTimeCounter = 1000L * DataTable[REG_VGNT_CONF_TIME] / TIMER0_PERIOD;
	ErrZoneMonitoring = 1000L * DataTable[REG_VGNT_VG_TIME_ERR_ZONE] / TIMER0_PERIOD;
	ResultPointOffset = DataTable[REG_VGNT_MEAS_POINT_OFFSET];
	VRate_mV_s = _FPtoIQ2(DataTable[ConfirmationMode ? REG_VGNT_CONF_VG_RATE : REG_VGNT_VG_RATE], 1000);
	Vg.ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), VRate_mV_s);

	BufferIsFull = FALSE;
	BufferPointer = 0;
	RingBuffer[0].Voltage = 0;
	RingBuffer[0].Current = 0;
}
// ----------------------------------------
