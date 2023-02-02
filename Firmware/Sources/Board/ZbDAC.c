// -----------------------------------------
// Driver for DAC via SPI
// ----------------------------------------

// Header
#include "ZbDAC.h"
//
#include "SysConfig.h"

// Forward functions
void ZbDAC_GateWriteX(Int16U Data);

// Functions
//
void ZbDAC_Init()
{
	// Init SPI-B (DAC)
	ZwSPIb_Init(TRUE, DAC_BAUDRATE, 16, DAC_PLR, DAC_PHASE, ZW_SPI_INIT_TX | ZW_SPI_INIT_CS, FALSE, FALSE);
	ZwSPIb_InitFIFO(0, 0);
	ZwSPIb_ConfigInterrupts(FALSE, FALSE);
	ZwSPIb_EnableInterrupts(FALSE, FALSE);
	
	// Init LDAC and multiplexer's pins
	ZwGPIO_WritePin(PIN_PDAC_LDAC, TRUE);
	ZwGPIO_WritePin(PIN_GDAC_LDAC, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W1, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W2, TRUE);
	//
	ZwGPIO_PinToOutput(PIN_PDAC_LDAC);
	ZwGPIO_PinToOutput(PIN_GDAC_LDAC);
	ZwGPIO_PinToOutput(PIN_DAC_W1);
	ZwGPIO_PinToOutput(PIN_DAC_W2);
}
// ----------------------------------------

void ZbDAC_GateWriteX(Int16U Data)
{
	// Select gate DAC
	ZwGPIO_WritePin(PIN_DAC_W2, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W1, FALSE);

	// Send data
	ZwSPIb_Send(&Data, 1, 16, STTNormal);
	DELAY_US(DAC_WRITE_DELAY_US);

	// Strobe to latch
	ZwGPIO_WritePin(PIN_GDAC_LDAC, FALSE);
	DELAY_US(1);
	ZwGPIO_WritePin(PIN_GDAC_LDAC, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W1, TRUE);
}
// ----------------------------------------

void ZbDAC_GateWriteVoltage(Int16U Voltage)
{
	// Config control bits
	Voltage &= 0x0FFF;
	Voltage |= BIT15;

	ZbDAC_GateWriteX(Voltage);
}
// ----------------------------------------

void ZbDAC_GateWriteCurrent(Int16U Current)
{
	// Config control bits
	Current &= 0x0FFF;

	ZbDAC_GateWriteX(Current);
}
// ----------------------------------------

void ZbDAC_DirectWriteX(Int16U Data)
{
	// Select gate DAC
	ZwGPIO_WritePin(PIN_DAC_W1, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W2, FALSE);

	// Send data
	ZwSPIb_Send(&Data, 1, 16, STTNormal);
	DELAY_US(DAC_WRITE_DELAY_US);

	// Strobe to latch
	ZwGPIO_WritePin(PIN_PDAC_LDAC, FALSE);
	DELAY_US(1);
	ZwGPIO_WritePin(PIN_PDAC_LDAC, TRUE);
	ZwGPIO_WritePin(PIN_DAC_W2, TRUE);
	DELAY_US(1);
}
// ----------------------------------------

void ZbDAC_DirectWriteVoltage(Int16U Voltage)
{
	// Config control bits
	Voltage &= 0x0FFF;
	Voltage |= BIT15;

	ZbDAC_DirectWriteX(Voltage);
}
// ----------------------------------------

void ZbDAC_DirectWriteCurrent(Int16U Current)
{
	// Config control bits
	Current &= 0x0FFF;

	ZbDAC_DirectWriteX(Current);
}
// ----------------------------------------

void ZbDAC_ForceOutputsToZero()
{
	ZbDAC_DirectWriteVoltage(0);
	ZbDAC_DirectWriteCurrent(0);

	ZbDAC_GateWriteVoltage(0);
	ZbDAC_GateWriteCurrent(0);
}
// ----------------------------------------
