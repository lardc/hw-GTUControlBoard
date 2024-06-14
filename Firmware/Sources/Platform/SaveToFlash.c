// Header
#include "SaveToFlash.h"

// Includes
#include "Flash.h"
#include "ZwSysCtrl.h"
#include "SysConfig.h"
#include "StorageDescription.h"

// Definitions
#define FLASH_RW_SECTOR				SECTORE
#define FLASH_RW_START_ADDR			0x3E4000
#define FLASH_WO_SECTOR				SECTORF
#define FLASH_WO_START_ADDR			0x3E0000

#define FLASH_SECTOR				SECTORH
#define FLASH_START_ADDR			0x3D8000 

// Forward functions
Int16U STF_StartAddressShift(Int16U Index, Boolean Readable);
Int16U STF_GetTypeLength(DataType CurrentType);
void STF_Save(Boolean Readable);

// Functions
//
void STF_AssignPointer(Int16U Index, Int32U Pointer)
{
	if(Index < StorageSize)
		TablePointers[Index] = Pointer;
}
// ----------------------------------------

void STF_LoadFromFlash()
{
	Int16U i, j;
	for(i = 0; i < StorageSize; i++)
	{
		if(TablePointers[i] && StorageDescription[i].UseRead)
		{
			Int32U ReadStartAddress = STF_StartAddressShift(i, TRUE) + MAX_DESCRIPTION_LEN + 4;
			for(j = 0; j < StorageDescription[i].Length * STF_GetTypeLength(StorageDescription[i].Type); j++)
				*(pInt16U)(TablePointers[i] + j) = *(pInt16U)(ReadStartAddress + j);
		}
	}
}
// ----------------------------------------

void STF_SaveUserData()
{
	STF_Save(TRUE);
}
// ----------------------------------------

void STF_SaveFaultData()
{
	STF_Save(FALSE);
}
// ----------------------------------------

void STF_Save(Boolean Readable)
{
	ZwSystem_DisableDog();
	DINT;
	Status = Flash_Erase(Readable ? FLASH_RW_SECTOR : FLASH_WO_SECTOR, (FLASH_ST *)&FlashStatus);

	Int16U i;
	for(i = 0; i < StorageSize; i++)
	{
		Int16U AddressShift = STF_StartAddressShift(i, Readable);
		static const Int16U DescriptionHeader[2] = {DT_Char, MAX_DESCRIPTION_LEN};

		// Запись заголовка описания
		Int32U StartAddr = Readable ? FLASH_RW_START_ADDR : FLASH_WO_START_ADDR;
		Status = Flash_Program((pInt16U)(StartAddr + AddressShift), (pInt16U)DescriptionHeader, 2,
				(FLASH_ST *)&FlashStatus);

		// Запись описания
		Status = Flash_Program((pInt16U)(StartAddr + AddressShift + 2), (pInt16U)StorageDescription[i].Description,
				MAX_DESCRIPTION_LEN, (FLASH_ST *)&FlashStatus);

		// Запись заголовка данных
		Int16U DataHeader[2] = {StorageDescription[i].Type, StorageDescription[i].Length};
		Status = Flash_Program((pInt16U)(StartAddr + AddressShift + 2 + MAX_DESCRIPTION_LEN),
				(pInt16U)DataHeader, 2, (FLASH_ST *)&FlashStatus);

		// Запись данных при наличии указателя
		if(TablePointers[i])
		{
			Int16U DataWriteLength = StorageDescription[i].Length * STF_GetTypeLength(StorageDescription[i].Type);
			Status = Flash_Program((pInt16U)(StartAddr + AddressShift + 2 + MAX_DESCRIPTION_LEN + DataWriteLength),
					(pInt16U)TablePointers[i], DataWriteLength, (FLASH_ST *)&FlashStatus);
		}
	}

	EINT;
	ZwSystem_EnableDog(SYS_WD_PRESCALER);
}
// ----------------------------------------

Int16U STF_GetTypeLength(DataType CurrentType)
{
	return (CurrentType == DT_Int32U || CurrentType == DT_Int32S || CurrentType == DT_Float) ? 2 : 1;
}
// ----------------------------------------

Int16U STF_StartAddressShift(Int16U Index, Boolean Readable)
{
	Int16U i, Shift = 0;
	for(i = 0; i < Index; i++)
	{
		if((Readable && StorageDescription[i].UseRead) || (!Readable && !StorageDescription[i].UseRead))
		{
			// На каждую запись выделяется дополнительно:
			// 2 поля для хранения типа и длины описания
			// 2 поля для хранения типа и длины самих данных
			Shift += STF_GetTypeLength(StorageDescription[i].Type) * StorageDescription[i].Length + MAX_DESCRIPTION_LEN + 4;
		}
	}
	return Shift;
}
// ----------------------------------------

void STF_SaveToFlash(Int16U Length, DataType Type, Int16S Data)
{
	Int16U DataLength = (Type == (DT_Int32U || DT_Int32S || DT_Float) ? 2 : 1) * Length;

	DataSegment NewDataSegment = { Length, Type, Data };

	pInt16U StoragePointer = (pInt16U)FLASH_START_ADDR;

	while (*StoragePointer != 0xFF)
		StoragePointer++;

	ZwSystem_DisableDog();
	DINT;
	Flash_Program(StoragePointer, (pInt16U)&NewDataSegment, 2 + DataLength, (FLASH_ST *)&FlashStatus);
	EINT;
	ZwSystem_EnableDog(SYS_WD_PRESCALER);
}
// ----------------------------------------

Int16S STF_Read()
{
	pInt16U StoragePointer = (pInt16U)FLASH_START_ADDR;
	StoragePointer++;
	StoragePointer++;
	return *StoragePointer;
}
// ----------------------------------------

void STF_EraseFlashDataSector()
{
	ZwSystem_DisableDog();
	DINT;
	Flash_Erase(FLASH_SECTOR, (FLASH_ST *)&FlashStatus);
	EINT;
	ZwSystem_EnableDog(SYS_WD_PRESCALER);
}
// ----------------------------------------
