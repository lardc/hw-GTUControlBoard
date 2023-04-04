// ----------------------------------------
// Driver for VRegister buffers via SPI
// ----------------------------------------

// Header
#include "ZbVRegister.h"
//
#include "SysConfig.h"
#include "ZwDSP.h"

// Constants
//
#define VR_WRITE_DELAY		50

// Functions
//
void ZbVRegister_Init()
{
	// Init SPI-C
	ZwSPIa_Init(TRUE, VR_BAUDRATE, 8, VR_PLR, VR_PHASE, ZW_SPI_INIT_TX | ZW_SPI_INIT_CS, FALSE, FALSE);
	ZwSPIa_InitFIFO(0, 0);
	ZwSPIa_ConfigInterrupts(FALSE, FALSE);
	ZwSPIa_EnableInterrupts(FALSE, FALSE);
	
	// Configure /OE
	ZwGPIO_WritePin(PIN_KREG_OE, TRUE);
	ZwGPIO_PinToOutput(PIN_KREG_OE);

	// Initialize register
	ZbVRegister_WriteRegister(0);
	// Enable /OE
	DELAY_US(VR_WRITE_DELAY);
	ZwGPIO_WritePin(PIN_KREG_OE, FALSE);
}
// ----------------------------------------

void ZbVRegister_WriteRegister(Int16U Value)
{
	ZwSPIa_Send((pInt16U)&Value, sizeof(Value), 8, STTNormal);
}
// ----------------------------------------

// No more.
