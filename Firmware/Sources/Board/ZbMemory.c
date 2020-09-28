// -----------------------------------------
// Driver for EEPROM via SPI
// ----------------------------------------

// Header
#include "ZbMemory.h"
#include "SysConfig.h"


// Constants
//
#define DATA_BUFFER_SIZE		16
#define EPROM_DATA_SEGMENT		4		// 4 * 2 + (3) < 16 - SPI FIFO max depth
#define EPROM_CL				8
#define EPROM_WRITE_DELAY_US	5000
//
// EPROM commands
#define EPROM_WRITE				0x02
#define EPROM_READ				0x03
#define EPROM_WREN				0x06


// Variables
//
static Int16U EPROMDataBuffer[DATA_BUFFER_SIZE];


// Forward functions
//
void ZbMEM_EnableWriteEPROM();


// Functions
//
void ZbMEM_Init()
{
	// Init SPI-D
	ZwSPId_Init(TRUE, EPROM_BAUDRATE, EPROM_CL, EPROM_PLR, EPROM_PHASE, ZW_SPI_INIT_TX | ZW_SPI_INIT_RX | ZW_SPI_INIT_CS, FALSE, FALSE);
	ZwSPId_InitFIFO(0, 0);
	ZwSPId_ConfigInterrupts(FALSE, FALSE);
	ZwSPId_EnableInterrupts(FALSE, FALSE);
	//
	ZwGPIO_WritePin(PIN_SPI_W1, FALSE);
	ZwGPIO_PinToOutput(PIN_SPI_W1);
}
// ----------------------------------------

void ZbMEM_WriteValuesEPROM(Int16U EPROMAddress, pInt16U Buffer, Int16U BufferSize)
{
	Int16U i, j, segCount;
	
	// Calculate segment count: only 16 SRAM bytes can be written per one transaction (FIFO limit)
	segCount = (BufferSize / EPROM_DATA_SEGMENT) + ((BufferSize % EPROM_DATA_SEGMENT) ? 1 : 0); 
		
	// Write segments
	for(j = 0; j < segCount; ++j)
	{
		Int16U dataSize;
		
		// Calculate address for next segment
		Int16U currentEPROMAddress = EPROMAddress + j * EPROM_DATA_SEGMENT * 2;

		// Enable writing for next operation
		ZbMEM_EnableWriteEPROM();

		// Write command ID
		EPROMDataBuffer[0] = EPROM_WRITE;
		// Memory address
		EPROMDataBuffer[1] = (currentEPROMAddress >> 8);
		EPROMDataBuffer[2] = currentEPROMAddress & 0x00FF;
		// Write data
		for(i = 0; i < MIN(BufferSize - j * EPROM_DATA_SEGMENT, EPROM_DATA_SEGMENT); ++i)
		{
			EPROMDataBuffer[3 + i * 2] = Buffer[j * EPROM_DATA_SEGMENT + i] >> 8;
			EPROMDataBuffer[3 + i * 2 + 1] = Buffer[j * EPROM_DATA_SEGMENT + i] & 0x00FF;
		}
	
		// Do SPI communication
		dataSize = 3 + MIN(BufferSize - j * EPROM_DATA_SEGMENT, EPROM_DATA_SEGMENT) * 2;
		ZwSPId_Send(EPROMDataBuffer, dataSize, EPROM_CL, STTNormal);

		DELAY_US(EPROM_WRITE_DELAY_US);
	}
}
// ----------------------------------------

void ZbMEM_ReadValuesEPROM(Int16U EPROMAddress, pInt16U Buffer, Int16U BufferSize)
{
	Int16U i, j, segCount, dataSize;

	// Calculate segment count: only 16 FRAM bytes can be read per one transaction (FIFO limit)
	segCount = (BufferSize / EPROM_DATA_SEGMENT) + ((BufferSize % EPROM_DATA_SEGMENT) ? 1 : 0); 
		
	// Read segments
	for(j = 0; j < segCount; ++j)
	{
		// Calculate address for next segment
		Int16U currentEPROMAddress = EPROMAddress + j * EPROM_DATA_SEGMENT * 2;
		
		// Write command ID
		EPROMDataBuffer[0] = EPROM_READ;
		// Memory address
		EPROMDataBuffer[1] = (currentEPROMAddress >> 8);
		EPROMDataBuffer[2] = currentEPROMAddress & 0x00FF;
		
		// Do SPI communication
		dataSize = 3 + MIN(BufferSize - j * EPROM_DATA_SEGMENT, EPROM_DATA_SEGMENT) * 2;
		ZwSPId_BeginReceive(EPROMDataBuffer, dataSize, EPROM_CL, STTNormal);
		while(ZwSPId_GetWordsToReceive() < dataSize)
			DELAY_US(1);
		ZwSPId_EndReceive(EPROMDataBuffer, dataSize);
		
		// Copy data
		for(i = 0; i < MIN(BufferSize - j * EPROM_DATA_SEGMENT, EPROM_DATA_SEGMENT); ++i)
		{
			Int16U result;
			
			// Get data from bytes
			result = (EPROMDataBuffer[3 + i * 2] & 0x00FF) << 8;
			result |= EPROMDataBuffer[3 + i * 2 + 1] & 0x00FF;
			
			Buffer[j * EPROM_DATA_SEGMENT + i] = result;
		}
	}	
}
// ----------------------------------------

void ZbMEM_EnableWriteEPROM()
{
	// Write @Enable@ command
	EPROMDataBuffer[0] = EPROM_WREN;
	// Do SPI communication
	ZwSPId_Send(EPROMDataBuffer, 1, EPROM_CL, STTNormal);
}
// ----------------------------------------

// No more.
