﻿#ifndef __SAVE_TO_FLASH_H
#define __SAVE_TO_FLASH_H

// Include
#include "stdinc.h"

// Definition
#define MAX_DESCRIPTION_LEN			64
#define MAX_CYCLE_DESCRIPTION_LEN	16

extern Int32U StoragePointer;

// Types
typedef enum __DataType
{
	DT_Char = 0,
	DT_Int8U,
	DT_Int8S,
	DT_Int16U,
	DT_Int16S,
	DT_Int32U,
	DT_Int32S,
	DT_Float
} DataType;

typedef struct __RecordDescription
{
	char Description[MAX_DESCRIPTION_LEN];
	DataType Type;
	Int16U Length;
	Boolean UseRead;
} RecordDescription;

typedef struct __CycleRecordDescription
{
	char Description[MAX_CYCLE_DESCRIPTION_LEN];
	// Cycle record data is always 32bit unsigned value
} CycleRecordDescription;

typedef struct __DataSegment
{
	Int16U Length;
	DataType Type;
	void* Data;
} DataSegment;

// Functions
void STF_AssignPointer(Int16U Index, Int32U Pointer);
void STF_SaveUserData();
void STF_SaveFaultData();
void STF_LoadFromFlash();

void STF_SaveToFlash(Int16U Length, DataType Type, void* Data);
void STF_SaveSymbol(Int32S Value);
Int16S STF_Read();
void STF_EraseFlashDataSector();

#endif // __SAVE_TO_FLASH_H
