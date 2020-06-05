// -----------------------------------------
// Controller logic
// ----------------------------------------

// Header
#include "Controller.h"
//
// Includes
#include "SysConfig.h"
//
#include "ZbBoard.h"
#include "DeviceObjectDictionary.h"
#include "DataTable.h"
#include "SCCISlave.h"
#include "DeviceProfile.h"
#include "Kelvin.h"
#include "Gate.h"
#include "Holding.h"
#include "Latching.h"
#include "RGate.h"
#include "VGNT.h"
#include "Calibrate.h"
#include "IQmathLib.h"
#include "Diagnostic.h"
#include "Regulator.h"
#include "MeasureUtils.h"

	
// Variables
//
DeviceStateCodes Codes = { WARNING_NONE, PROBLEM_NONE, FAULT_NONE };
volatile Int64U CONTROL_TimeCounter = 0;
volatile Int64U FanTurnOff_Counter = 0;
volatile DeviceState CONTROL_State = DS_None;
Boolean CycleActive = FALSE;
//
volatile Int16U CONTROL_Values_Counter = 0;

// Boot-loader flag
#pragma DATA_SECTION(CONTROL_BootLoaderRequest, "bl_flag");
volatile Int16U CONTROL_BootLoaderRequest = 0;


// Forward functions
//
void CONTROL_SetDeviceState(DeviceState NewState);
void CONTROL_FillWithDefaults();
void CONTROL_HaltExecution();
void CONTROL_Execute(Int16U ActionID, pInt16U UserError);
Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U UserError);


// Functions
//
void CONTROL_Init(Boolean BadClockDetected)
{
	pInt16U cnt = (pInt16U)&CONTROL_Values_Counter;

	// Variables for endpoint configuration
	Int16U EPIndexes[EP_COUNT] = { EP16_Data_Vg, EP16_Data_Ig, EP16_Data_Vd, EP16_Data_Id,
								   EP16_Control_Vg, EP16_Control_Ig, EP16_Control_Vd, EP16_Control_Id,
								   EP16_Target_Vg, EP16_Target_Ig, EP16_Target_Vd, EP16_Target_Id };

	Int16U EPSized[EP_COUNT] = { VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE,
								 VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE,
								 VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE, VALUES_x_SIZE };

	pInt16U EPCounters[EP_COUNT] = { cnt, cnt, cnt, cnt,
									 cnt, cnt, cnt, cnt,
									 cnt, cnt, cnt, cnt };

	pInt16U EPDatas[EP_COUNT] = { CONTROL_Values_Vg, CONTROL_Values_Ig, CONTROL_Values_Vd, CONTROL_Values_Id,
								  CONTROL_Values_Ctrl_Vg, CONTROL_Values_Ctrl_Ig, CONTROL_Values_Ctrl_Vd, CONTROL_Values_Ctrl_Id,
								  CONTROL_Values_Trgt_Vg, CONTROL_Values_Trgt_Ig, CONTROL_Values_Trgt_Vd, CONTROL_Values_Trgt_Id };

	// Data-table EPROM service configuration
	EPROMServiceConfig EPROMService = { &ZbMEM_WriteValuesEPROM, &ZbMEM_ReadValuesEPROM };

	// Init data table
	DT_Init(EPROMService, BadClockDetected);
	// Fill state variables with default values
	CONTROL_FillWithDefaults();
	CONTROL_SetDeviceState(DS_None);

	// Device profile initialization
	DEVPROFILE_Init(&CONTROL_DispatchAction, &CycleActive);
	DEVPROFILE_InitEPService(EPIndexes, EPSized, EPCounters, EPDatas);
	// Reset control values
	DEVPROFILE_ResetControlSection();

	// Use quadratic correction for block
	DataTable[REG_QUADRATIC_CORR] = 1;

	if(!BadClockDetected)
	{
		if(ZwSystem_GetDogAlarmFlag())
		{
			DataTable[REG_WARNING] = WARNING_WATCHDOG_RESET;
			ZwSystem_ClearDogAlarmFlag();
		}
	}
	else
	{
		CycleActive = TRUE;
		DataTable[REG_DISABLE_REASON] = DISABLE_BAD_CLOCK;
		CONTROL_SetDeviceState(DS_Disabled);
	}
}
// ----------------------------------------

void CONTROL_Idle()
{
	DEVPROFILE_ProcessRequests();
	DEVPROFILE_UpdateCANDiagStatus();
}
// ----------------------------------------

void CONTROL_UpdateHigh()
{
	if (CycleActive)
	{
		Boolean IsComplited = FALSE;
		CombinedData MeasureSample;

		// Выполнение регулирования
		REGULATOR_Cycle(MeasureSample = MU_HandleADC(ZwADC_GetValues1()));

		// Логгирование результата
		MU_LogScope(MeasureSample);

		switch (CONTROL_State)
		{
			case DS_Kelvin:
				IsComplited = KELVIN_Process(&Codes);
				break;

			case DS_Gate:
				IsComplited = GATE_Process(MeasureSample, &Codes);
				break;

			case DS_IH:
				IsComplited = HOLDING_Process(MeasureSample, &Codes);
				break;

			case DS_IL:
				IsComplited = LATCHING_Process(MeasureSample, &Codes);
				break;

			case DS_RG:
				IsComplited = RGATE_Process(MeasureSample, &Codes);
				break;

			case DS_Calibrate:
				IsComplited = CALIBRATE_Process(MeasureSample, &Codes);
				break;

			case DS_Vgnt:
				IsComplited = VGNT_Process(MeasureSample, &Codes);
				break;
		}

		if (IsComplited)
		{
			if (Codes.FaultReason != FAULT_NONE)
			{
				DataTable[REG_FAULT_REASON] = Codes.FaultReason;
				CONTROL_SetDeviceState(DS_Fault);
			}
			else
				CONTROL_SetDeviceState(DS_None);

			CycleActive = FALSE;
			DataTable[REG_PROBLEM] = Codes.Problem;
			DataTable[REG_WARNING] = Codes.Warning;
			DataTable[REG_TEST_FINISHED] = (Codes.Problem == PROBLEM_NONE) ? OPRESULT_OK : OPRESULT_FAIL;

			ZbGPIO_LED1(FALSE);
		}	
	}				
}
// ----------------------------------------

void CONTROL_NotifyCANFault(ZwCAN_SysFlags Flag)
{
	DEVPROFILE_NotifyCANFault(Flag);
}
// ----------------------------------------

void CONTROL_SetDeviceState(DeviceState NewState)
{
	// Set new state
	CONTROL_State = NewState;
	DataTable[REG_DEV_STATE] = NewState;
}
// ----------------------------------------

void CONTROL_FillWithDefaults()
{
	DEVPROFILE_ResetEPReadState();
	DEVPROFILE_ResetScopes(0);

	// Set states
	DataTable[REG_TEST_FINISHED] = OPRESULT_NONE;
	DataTable[REG_FAULT_REASON] = FAULT_NONE;
	DataTable[REG_DISABLE_REASON] = DISABLE_NONE;
	DataTable[REG_WARNING] = WARNING_NONE;

	// Set results to zero
	DataTable[REG_RESULT_KELVIN] = 0;
	DataTable[REG_RESULT_IGT] = 0;
	DataTable[REG_RESULT_VGT] = 0;
	DataTable[REG_RESULT_IH] = 0;
	DataTable[REG_RESULT_IL] = 0;
	DataTable[REG_RESULT_RG] = 0;
	DataTable[REG_RESULT_CAL] = 0;
	//
	DataTable[REG_KELVIN_1_2] = 0;
	DataTable[REG_KELVIN_4_1] = 0;
	DataTable[REG_KELVIN_1_4] = 0;
	DataTable[REG_KELVIN_3_2] = 0;
	//
	DataTable[REG_PROCESS_COUNTER] = 0;
}
// ----------------------------------------

void CONTROL_HaltExecution()
{
	ZwTimer_StopT0();
	CycleActive = FALSE;
	CONTROL_SetDeviceState(DS_None);

	ZbGPIO_DirectEnableOutput(FALSE);
	ZbGPIO_GateEnableOutput(FALSE);
	ZbDAC_ForceOutputsToZero();
	ZbGPIO_LED1(FALSE);
}
// ----------------------------------------

void CONTROL_Execute(Int16U ActionID, pInt16U UserError)
{
	if (CONTROL_State == DS_None)
	{
		CONTROL_FillWithDefaults();
		CycleActive = TRUE;
		ZbGPIO_LED1(TRUE);

		switch (ActionID)
		{
			case ACT_START_KELVIN:
				CONTROL_SetDeviceState(DS_Kelvin);
				KELVIN_Prepare();
				break;

			case ACT_START_GATE:
				CONTROL_SetDeviceState(DS_Gate);
				GATE_Prepare();
				break;

			case ACT_START_VGNT:
				CONTROL_SetDeviceState(DS_Vgnt);
				VGNT_Prepare();
				break;

			case ACT_START_IH:
				CONTROL_SetDeviceState(DS_IH);
				HOLDING_Prepare();
				break;

			case ACT_START_IL:
				CONTROL_SetDeviceState(DS_IL);
				LATCHING_Prepare();
				break;

			case ACT_START_RG:
				CONTROL_SetDeviceState(DS_RG);
				RGATE_Prepare();
				break;

			case ACT_START_CAL_VG:
				CONTROL_SetDeviceState(DS_Calibrate);
				CALIBRATE_Prepare(SelectVg);
				break;

			case ACT_START_CAL_IG:
				CONTROL_SetDeviceState(DS_Calibrate);
				CALIBRATE_Prepare(SelectIg);
				break;

			case ACT_START_CAL_VD:
				CONTROL_SetDeviceState(DS_Calibrate);
				CALIBRATE_Prepare(SelectVd);
				break;

			case ACT_START_CAL_ID:
				CONTROL_SetDeviceState(DS_Calibrate);
				CALIBRATE_Prepare(SelectId);
				break;
		}
	}
	else
		*UserError = ERR_OPERATION_BLOCKED;
}
// ----------------------------------------

Boolean CONTROL_DispatchAction(Int16U ActionID, pInt16U UserError)
{
	switch (ActionID)
	{
		case ACT_START_KELVIN:
		case ACT_START_GATE:
		case ACT_START_IH:
		case ACT_START_IL:
		case ACT_START_RG:
		case ACT_START_CAL_VG:
		case ACT_START_CAL_IG:
		case ACT_START_CAL_VD:
		case ACT_START_CAL_ID:
		case ACT_START_VGNT:
			CONTROL_Execute(ActionID, UserError);
			break;

		case ACT_STOP_TEST:
			if (CONTROL_State != DS_None && CONTROL_State != DS_Fault)
			{
				CONTROL_HaltExecution();
				DataTable[OPRESULT_FAIL] = OPRESULT_FAIL;
				DataTable[REG_PROBLEM] = PROBLEM_OPERATION_STOPPED;
			}
			break;

		case ACT_CLR_FAULT:
			if (CONTROL_State == DS_Fault)
			{
				CONTROL_SetDeviceState(DS_None);
				DataTable[REG_FAULT_REASON] = FAULT_NONE;
			}
			break;

		case ACT_CLR_WARNING:
			DataTable[REG_WARNING] = WARNING_NONE;
			break;

		default:
			return DIAG_Process(ActionID);
	}

	return TRUE;
}
// ----------------------------------------

void FAN_Control()
{
	if (CONTROL_State == DS_Vgnt)
	{
		ZbGPIO_FAN(TRUE);
		FanTurnOff_Counter = CONTROL_TimeCounter + DataTable[REG_FAN_OFF_DELAY];
	}
	else
		if (CONTROL_TimeCounter >= FanTurnOff_Counter)
			ZbGPIO_FAN(FALSE);
}
// ----------------------------------------
