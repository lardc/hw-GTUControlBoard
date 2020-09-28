// -----------------------------------------
// Utility functions for parameters conversion
// ----------------------------------------

// Header
#include "ConvertUtils.h"
//
// Includes
#include "DeviceObjectDictionary.h"
#include "DataTable.h"
#include "Controller.h"
#include "stdlib.h"
#include "IQmathLib.h"
#include "IQmathUtils.h"


// Definitions
#define DAC_MAX_VAL		0x0FFF


// Types
typedef struct __DACConvParameters
{
	_iq P2;
	_iq P1;
	_iq P0;
	_iq K;
	_iq B;
} DACConvParameters, *pDACConvParameters;


// Variables
static DACConvParameters ParamsGateV, ParamsGateI, ParamsDirectV, ParamsDirectI;


// Forward functions
Int16U CU_XtoDAC(_iq Value, DACConvParameters Parameters);
DACConvParameters CU_LoadParams(Int16U RegK, Int16U RegB, Int16U RegP2, Int16U RegP1, Int16U RegP0);
DACConvParameters CU_LoadParams_Simple(Int16U RegK, Int16U RegB);


// Functions
//
void CU_Cache()
{
	// Цепь управления - напряжение
	ParamsGateV  = CU_LoadParams_Simple(REG_DAC_VG_CONV_K, REG_DAC_VG_CONV_B);

	// Цепь управления - ток
	ParamsGateI  = CU_LoadParams(REG_DAC_IG_CONV_K, REG_DAC_IG_CONV_B, REG_DAC_IG_FINE_P2, REG_DAC_IG_FINE_P1, REG_DAC_IG_FINE_P0);

	// Силовая цепь - напряжение
	ParamsDirectV = CU_LoadParams_Simple(REG_DAC_VD_CONV_K, REG_DAC_VD_CONV_B);

	// Силовая цепь - ток
	ParamsDirectI = CU_LoadParams(REG_DAC_ID_CONV_K, REG_DAC_ID_CONV_B, REG_DAC_ID_FINE_P2, REG_DAC_ID_FINE_P1, REG_DAC_ID_FINE_P0);
}
// ----------------------------------------

Int16U CU_GateVtoDAC(_iq Value)
{
	return CU_XtoDAC(Value, ParamsGateV);
}
// ----------------------------------------

Int16U CU_GateItoDAC(_iq Value)
{
	return CU_XtoDAC(Value, ParamsGateI);
}
// ----------------------------------------

Int16U CU_DirectVtoDAC(_iq Value)
{
	return CU_XtoDAC(Value, ParamsDirectV);
}
// ----------------------------------------

Int16U CU_DirectItoDAC(_iq Value)
{
	return CU_XtoDAC(Value, ParamsDirectI);
}
// ----------------------------------------

DACConvParameters CU_LoadParams(Int16U RegK, Int16U RegB, Int16U RegP2, Int16U RegP1, Int16U RegP0)
{
	DACConvParameters ret;

	ret.P2	= _IQI((Int16S)DataTable[RegP2]);
	ret.P1	= _FPtoIQ2(DataTable[RegP1], 1000);
	ret.P0	= _IQI((Int16S)DataTable[RegP0]);

	ret.K	= _FPtoIQ2(DataTable[RegK], 1000);
	ret.B	= _IQI((Int16S)DataTable[RegB]);

	return ret;
}
// ----------------------------------------

DACConvParameters CU_LoadParams_Simple(Int16U RegK, Int16U RegB)
{
	DACConvParameters ret;

	ret.P2	= 0;
	ret.P1	= _IQ(1.0f);
	ret.P0	= 0;

	ret.K	= _FPtoIQ2(DataTable[RegK], 1000);
	ret.B	= _IQI((Int16S)DataTable[RegB]);

	return ret;
}
// ----------------------------------------

Int16U CU_XtoDAC(_iq Value, DACConvParameters Parameters)
{
	_iq tmp = _IQdiv(Value, _IQ(1000.0f));
	_iq tmp2 = _IQmpy(tmp, _IQmpy(tmp, Parameters.P2)) + _IQmpy(Value, Parameters.P1) + Parameters.P0;

	Int16S val = _IQint(_IQmpy((tmp2 + Parameters.B), Parameters.K));

	if (val > DAC_MAX_VAL)
		return DAC_MAX_VAL;
	else if (val < 0)
		return 0;
	else
		return val;
}
// ----------------------------------------
