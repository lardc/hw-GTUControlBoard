// -----------------------------------------
// Diagnostic tools
// ----------------------------------------

// Header
#include "Diagnostic.h"
//
#include "Controller.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "ZbBoard.h"
#include "Kelvin.h"


// Functions
//
Boolean DIAG_Process(Int16U Command)
{
	switch (Command)
	{
		case ACT_DBG_DAC_CURR_GATE:
			if(CONTROL_State == DS_None)
				ZbDAC_GateWriteCurrent(DataTable[REG_DBG]);
			break;

		case ACT_DBG_DAC_CURR_DIRECT:
			if(CONTROL_State == DS_None)
				ZbDAC_DirectWriteCurrent(DataTable[REG_DBG]);
			break;

		case ACT_DBG_KELVIN:
			if(CONTROL_State == DS_None)
				KELVIN_Diag(DataTable[REG_DBG]);
			break;

		case ACT_DBG_DAC_VOLT_GATE:
			if(CONTROL_State == DS_None)
				ZbDAC_GateWriteVoltage(DataTable[REG_DBG]);
			break;

		case ACT_DBG_DAC_VOLT_DIRECT:
			if(CONTROL_State == DS_None)
				ZbDAC_DirectWriteVoltage(DataTable[REG_DBG]);
			break;

		case ACT_DBG_OUTPUT_CTRL_GATE:
			if(CONTROL_State == DS_None)
				ZbGPIO_GateEnableOutput(DataTable[REG_DBG]);
			break;

		case ACT_DBG_OUTPUT_CTRL_DIRECT:
			if(CONTROL_State == DS_None)
				ZbGPIO_DirectEnableOutput(DataTable[REG_DBG]);
			break;

		case ACT_DBG_SYNC_OSC:
			if(CONTROL_State == DS_None)
				ZbGPIO_SyncOSC(DataTable[REG_DBG]);
			break;

		case ACT_DBG_FAN:
			if(CONTROL_State == DS_None)
				ZbGPIO_FAN(DataTable[REG_DBG]);
			break;

		case ACT_DBG_GATE_PS_TUNE:
			if(CONTROL_State == DS_None)
				ZbGPIO_GatePSTune(DataTable[REG_DBG]);
			break;

		default:
			return FALSE;
	}

	return TRUE;
}
// ----------------------------------------
