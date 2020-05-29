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
	// Управление анодной цепью
	ZwGPIO_WritePin(PIN_POWER_ST, FALSE);
	ZwGPIO_WritePin(PIN_POWER_EN_LGC, FALSE);
	ZwGPIO_WritePin(PIN_POWER_IFB_LGC, FALSE);

	// Управление цепью управления
	ZwGPIO_WritePin(PIN_GATE_ST, FALSE);
	ZwGPIO_WritePin(PIN_GATE_EN_LGC, FALSE);
	ZwGPIO_WritePin(PIN_GATE_IFB_LGC, FALSE);

	// Линии индикации
	ZwGPIO_WritePin(PIN_LED_1, FALSE);
	ZwGPIO_WritePin(PIN_LED_2, FALSE);


	// Настройка в режиме выхода
	ZwGPIO_PinToOutput(PIN_POWER_ST);
	ZwGPIO_PinToOutput(PIN_POWER_EN_LGC);
	ZwGPIO_PinToOutput(PIN_POWER_IFB_LGC);

	ZwGPIO_PinToOutput(PIN_GATE_ST);
	ZwGPIO_PinToOutput(PIN_GATE_EN_LGC);
	ZwGPIO_PinToOutput(PIN_GATE_IFB_LGC);

	ZwGPIO_PinToOutput(PIN_LED_1);
	ZwGPIO_PinToOutput(PIN_LED_2);


   	// Настройка в режиме входа
   	ZwGPIO_PinToInput(PIN_VRF_SENSE, TRUE, GPIO_NSAMPLE);
   	ZwGPIO_PinToInput(PIN_EXT_SYNC, TRUE, GPIO_NSAMPLE);
}
// ----------------------------------------

Boolean ZbGPIO_ExtSyncReadState()
{
	return ZwGPIO_ReadPin(PIN_EXT_SYNC);
}
// ----------------------------------------

void ZbGPIO_DirectEnableSelfTest(Boolean State)
{
	ZwGPIO_WritePin(PIN_POWER_ST, State);
}
// ----------------------------------------

void ZbGPIO_DirectEnableOutput(Boolean State)
{
	ZwGPIO_WritePin(PIN_POWER_EN_LGC, State);
}
// ----------------------------------------

void ZbGPIO_DirectLockFeedback(Boolean State)
{
	ZwGPIO_WritePin(PIN_POWER_IFB_LGC, State);
}
// ----------------------------------------

void ZbGPIO_GateEnableSelfTest(Boolean State)
{
	ZwGPIO_WritePin(PIN_GATE_ST, State);
}
// ----------------------------------------

void ZbGPIO_GateEnableOutput(Boolean State)
{
	ZwGPIO_WritePin(PIN_GATE_EN_LGC, State);
}
// ----------------------------------------

void ZbGPIO_GateLockFeedback(Boolean State)
{
	ZwGPIO_WritePin(PIN_GATE_IFB_LGC, State);
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
