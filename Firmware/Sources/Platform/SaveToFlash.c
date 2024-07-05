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
Int16U STF_StartAddressShift(Int16U Index);
Int16U STF_GetTypeLength(DataType CurrentType);
void STF_Save();
Int32U STF_ShiftStorageEnd();

// Functions
//
void STF_AssignPointer(Int16U Index, Int32U Pointer)
{
	if(Index < StorageSize)
		TablePointers[Index] = Pointer;
}
// ----------------------------------------

/*
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
*/

void STF_SaveUserData()
{
	STF_Save();
}
// ----------------------------------------

void STF_SaveFaultData()
{
	STF_Save();
}
// ----------------------------------------

void STF_Save()
{
	ZwSystem_DisableDog();
	DINT;

	Int32U StartAddr = STF_ShiftStorageEnd();

	Int16U i;
	for(i = 0; i < StorageSize; i++)
	{
		static Int16U DescriptionHeader[2] = {DT_Char, MAX_DESCRIPTION_LEN};

		// Запись заголовка описания
		Status = Flash_Program((pInt16U)StartAddr, (pInt16U)DescriptionHeader, 2,
				(FLASH_ST *)&FlashStatus);
		StartAddr += 2;

		// Запись описания
		Status = Flash_Program((pInt16U)StartAddr, (pInt16U)StorageDescription[i].Description,
				MAX_DESCRIPTION_LEN, (FLASH_ST *)&FlashStatus);
		StartAddr += MAX_DESCRIPTION_LEN;

		// Запись заголовка данных
		Int16U DataHeader[2] = {StorageDescription[i].Type, StorageDescription[i].Length};
		Status = Flash_Program((pInt16U)StartAddr,
				(pInt16U)DataHeader, 2, (FLASH_ST *)&FlashStatus);
		StartAddr += 2;

		// Запись данных при наличии указателя
		if(TablePointers[i])
		{
			Int16U DataWriteLength = StorageDescription[i].Length * STF_GetTypeLength(StorageDescription[i].Type);
			Status = Flash_Program((pInt16U)StartAddr,
					(pInt16U)TablePointers[i], DataWriteLength, (FLASH_ST *)&FlashStatus);
			StartAddr += DataWriteLength;
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

Int16U STF_StartAddressShift(Int16U Index)
{
	Int16U i, Shift = 0;
	for(i = 0; i < Index; i++)
	{
		// На каждую запись выделяется дополнительно:
		// 2 поля для хранения типа и длины описания
		// 2 поля для хранения типа и длины самих данных
		Shift += STF_GetTypeLength(StorageDescription[i].Type) * StorageDescription[i].Length + MAX_DESCRIPTION_LEN + 4;
	}
	return Shift;
}
// ----------------------------------------

Int32U STF_ShiftStorageEnd() {
	Int32U StoragePointer = FLASH_START_ADDR;

	while (*(pInt16U)StoragePointer != 0xFFFF)
	{
		Int16U CurrentType = *(pInt16U)StoragePointer;

		if (CurrentType > DT_Float)
			break;

		Int16U TypeLength = STF_GetTypeLength(CurrentType);
		++StoragePointer;

		Int16U Length = *(pInt16U)StoragePointer * (Int16U)TypeLength;
		StoragePointer += Length + 1;
	}
	return StoragePointer;
}
// ----------------------------------------

void STF_EraseDataSector()
{
	ZwSystem_DisableDog();
	DINT;
	Flash_Erase(FLASH_SECTOR, (FLASH_ST *)&FlashStatus);
	EINT;
	ZwSystem_EnableDog(SYS_WD_PRESCALER);
}
// ----------------------------------------
