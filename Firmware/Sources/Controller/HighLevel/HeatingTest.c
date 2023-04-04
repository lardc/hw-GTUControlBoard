// ----------------------------------------
// Logic of Gate calibration
// ----------------------------------------

// Header
#include "HeatingTest.h"
//
// Includes
#include "SysConfig.h"
#include "ZbBoard.h"
#include "DataTable.h"
#include "DeviceObjectDictionary.h"
#include "Global.h"
#include "IQmathUtils.h"
#include "MeasureUtils.h"
#include "Regulator.h"
#include "Common.h"


// Types
//
typedef enum __CalibrateState
{
	HEATING_TEST_STATE_RISE = 0,
	HEATING_TEST_STATE_STAB,
} CalibrateState;


// Variables
//
static CalibrateState State;
static CommonSettings LogicSettings;
static ChannelSettings Vg = {0}, Ig = {0}, Vd = {0}, Id = {0};
static _iq xLimit, xSetpoint, xChangeStep;

// Forward functions
//
void HEATING_TEST_CacheVariables();


// Functions
//
void HEATING_TEST_Prepare()
{
	// Кэширование переменных
	HEATING_TEST_CacheVariables();
	COMMON_PrepareStart();

	// Активация регуляторов
	REGULATOR_Enable(SelectVg, TRUE);
	REGULATOR_Enable(SelectIg, TRUE);

	// Выставление начальных значений
	REGULATOR_Update(SelectVg, 0);
	REGULATOR_Update(SelectIg, Ig.Limit);

	State = HEATING_TEST_STATE_RISE;
	ZwTimer_StartT0();
}
// ----------------------------------------

Boolean HEATING_TEST_Process(CombinedData MeasureSample, pDeviceStateCodes Codes)
{
	switch (State)
	{
		// Нарастание основного сигнала
		case HEATING_TEST_STATE_RISE:
			{
				xSetpoint += xChangeStep;
				if (xSetpoint > xLimit)
				{
					xSetpoint = xLimit;

					State = HEATING_TEST_STATE_STAB;
				}
				REGULATOR_Update(SelectVg, xSetpoint);
			}
			break;

		// Задержка на стабилизацию основного сигнала
		case HEATING_TEST_STATE_STAB:
			{
			}
			break;
	}

	return FALSE;
}
// ----------------------------------------

void HEATING_TEST_CacheVariables()
{
	COMMON_CacheVariables(&Vd, &Id, &Vg, &Ig, &LogicSettings);

	xLimit = Vg.Limit;
	xChangeStep = _IQmpy(_FPtoIQ2(TIMER0_PERIOD, 1000), _IQI(DataTable[REG_CAL_V_RISE_RATE]));

	xSetpoint = 0;
}
// ----------------------------------------
