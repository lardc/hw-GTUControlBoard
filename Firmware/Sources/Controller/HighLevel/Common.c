// -----------------------------------------
// Common high-level tools
// ----------------------------------------

// Header
#include "Common.h"
//
// Includes
#include "ZbBoard.h"
#include "SysConfig.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Regulator.h"
#include "MeasureUtils.h"
#include "ConvertUtils.h"


// Forward functions
//
void COMMON_ClearVar(pChannelSettings Variable);


// Functions
//
void COMMON_PrepareStart()
{
	// Кэширование коэффициентов пересчёта
	MU_Cache();
	CU_Cache();

	// Инициализация регуляторов
	REGULATOR_InitAll();
	ZbDAC_ForceOutputsToZero();

	// Активация системы
	ZbGPIO_GateEnableOutput(TRUE);
	//
	ZbGPIO_DirectEnableOutput(TRUE);
}
// ----------------------------------------

void COMMON_ClearVar(pChannelSettings Variable)
{
	Variable->Limit = 0;
	Variable->ChangeStep = 0;
	Variable->Setpoint = 0;
}
// ----------------------------------------

void COMMON_CacheVariables(pChannelSettings _Vd, pChannelSettings _Id, pChannelSettings _Vg, pChannelSettings _Ig,
						   pCommonSettings _LogicSettings)
{
	// Очистка переменных
	COMMON_ClearVar(_Vd);
	COMMON_ClearVar(_Id);
	COMMON_ClearVar(_Vg);
	COMMON_ClearVar(_Ig);

	// Присвоение предельных значений
	_Vd->Limit = _IQI(DataTable[REG_V_DIRECT_LIMIT]);
	_Id->Limit = _IQI(DataTable[REG_I_DIRECT_LIMIT]);
	_Vg->Limit = _IQI(DataTable[REG_V_GATE_LIMIT]);
	_Ig->Limit = _IQI(DataTable[REG_I_GATE_LIMIT]);

	// Шаг нарастания анодного напряжения
	_Vd->ChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_COMM_VD_RISE_RATE]));

	// Общие настройки логики
	_LogicSettings->CycleCounter	= 0;
	_LogicSettings->StabCounter		= 1000L * DataTable[REG_COMM_STAB_TIME] / TIMER0_PERIOD;
	_LogicSettings->IdLeak 			= _IQI(DataTable[REG_COMM_ID_LEAK_MAX]);
	_LogicSettings->AllowedError	= _FPtoIQ2(DataTable[REG_COMM_STAB_ERROR], 100);
}
// ----------------------------------------

void COMMON_PrepareFinish()
{
	REGULATOR_DisableAll();
	ZbDAC_ForceOutputsToZero();
}
// ----------------------------------------

void COMMON_Finish()
{
	ZbGPIO_GateEnableOutput(FALSE);
	ZbGPIO_DirectEnableOutput(FALSE);
	ZwTimer_StopT0();
}
// ----------------------------------------
