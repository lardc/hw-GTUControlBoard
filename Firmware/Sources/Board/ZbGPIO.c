// -----------------------------------------
// Board-specific GPIO functions
// ----------------------------------------

// Header
#include "ZbGPIO.h"
//
#include "SysConfig.h"


// Functions
//
void ZbGPIO_Init()
{
	// Настройка в режиме выхода
	ZwGPIO_WritePin(PIN_MP_ENABLE_LGC, FALSE);
	ZwGPIO_WritePin(PIN_GATE_ENABLE_LGC, FALSE);
	ZwGPIO_WritePin(PIN_LED_1, FALSE);
	ZwGPIO_WritePin(PIN_LED_2, FALSE);
	ZwGPIO_WritePin(PIN_GATE_PS_TUNE_EN_LGC, FALSE);
	ZwGPIO_WritePin(PIN_SYNC_OSC, FALSE);
	ZwGPIO_WritePin(PIN_FAN, FALSE);

	ZwGPIO_PinToOutput(PIN_MP_ENABLE_LGC);
	ZwGPIO_PinToOutput(PIN_GATE_ENABLE_LGC);
	ZwGPIO_PinToOutput(PIN_LED_1);
	ZwGPIO_PinToOutput(PIN_LED_2);
	ZwGPIO_PinToOutput(PIN_GATE_PS_TUNE_EN_LGC);
	ZwGPIO_PinToOutput(PIN_SYNC_OSC);
	ZwGPIO_PinToOutput(PIN_FAN);

   	// Настройка в режиме входа
   	ZwGPIO_PinToInput(PIN_VRF_SENSE, TRUE, GPIO_NSAMPLE);
   	ZwGPIO_PinToInput(PIN_EXT_SYNC, TRUE, GPIO_NSAMPLE);
}
// ----------------------------------------

Boolean ZbGPIO_ExtSyncTrig()
{
	return !ZwGPIO_ReadPin(PIN_EXT_SYNC);
}
// ----------------------------------------

void ZbGPIO_SyncOSC(Boolean State)
{
	ZwGPIO_WritePin(PIN_SYNC_OSC, State);
}
// ----------------------------------------

void ZbGPIO_GatePSTune(Boolean State)
{
	ZwGPIO_WritePin(PIN_GATE_PS_TUNE_EN_LGC, State);
}
// ----------------------------------------

void ZbGPIO_FAN(Boolean State)
{
	ZwGPIO_WritePin(PIN_FAN, State);
}
// ----------------------------------------

void ZbGPIO_DirectEnableOutput(Boolean State)
{
	ZwGPIO_WritePin(PIN_MP_ENABLE_LGC, State);
}
// ----------------------------------------

void ZbGPIO_GateEnableOutput(Boolean State)
{
	ZwGPIO_WritePin(PIN_GATE_ENABLE_LGC, State);
}
// ----------------------------------------

Boolean ZbGPIO_KelvinProbe()
{
	return !ZwGPIO_ReadPin(PIN_VRF_SENSE);
}
// ----------------------------------------

void ZbGPIO_LED1(Boolean State)
{
	ZwGPIO_WritePin(PIN_LED_1, State);
}
// ----------------------------------------

void ZbGPIO_LED2(Boolean State)
{
	ZwGPIO_WritePin(PIN_LED_2, State);
}
// ----------------------------------------

void ZbGPIO_ToggleLED2()
{
	ZwGPIO_TogglePin(PIN_LED_2);
}
// ----------------------------------------

// No more.
