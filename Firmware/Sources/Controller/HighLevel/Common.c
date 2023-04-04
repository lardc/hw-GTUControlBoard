// ----------------------------------------
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
void COMMON_CacheVariablesX(pChannelSettings _Vd, pChannelSettings _Id, pChannelSettings _Vg, pChannelSettings _Ig,
						   pCommonSettings _LogicSettings, Boolean IsCalibration);


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
	ZbGPIO_DirectEnableOutput(TRUE);

	// Синхросигнал для осциллографа
	ZbGPIO_SyncOSC(TRUE);
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
	COMMON_CacheVariablesX(_Vd, _Id, _Vg, _Ig, _LogicSettings, FALSE);
}
// ----------------------------------------

void COMMON_CacheCalibrationVariables(pChannelSettings _Vd, pChannelSettings _Id, pChannelSettings _Vg, pChannelSettings _Ig,
						   pCommonSettings _LogicSettings)
{
	COMMON_CacheVariablesX(_Vd, _Id, _Vg, _Ig, _LogicSettings, TRUE);
}
// ----------------------------------------

void COMMON_CacheVariablesX(pChannelSettings _Vd, pChannelSettings _Id, pChannelSettings _Vg, pChannelSettings _Ig,
						   pCommonSettings _LogicSettings, Boolean IsCalibration)
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
	_LogicSettings->AllowedError	= _FPtoIQ2(DataTable[IsCalibration ? REG_COMM_CAL_STAB_ERROR : REG_COMM_STAB_ERROR], 100);
	_LogicSettings->VgMinInput		= _IQI(DataTable[REG_COMM_VG_MIN_INPUT]);
}
// ----------------------------------------

void COMMON_PrepareFinish()
{
	REGULATOR_DisableAll();
	ZbDAC_ForceOutputsToZero();
	ZbGPIO_GatePSTune(FALSE);
	ZbGPIO_SyncOSC(FALSE);
}
// ----------------------------------------

void COMMON_Finish()
{
	ZbGPIO_GateEnableOutput(FALSE);
	ZbGPIO_DirectEnableOutput(FALSE);
	ZbGPIO_LED1(FALSE);
	ZwTimer_StopT0();
}
// ----------------------------------------
