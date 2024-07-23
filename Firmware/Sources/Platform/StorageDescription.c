// Header
#include "StorageDescription.h"
#include "Global.h"

// Variables
RecordDescription StorageDescription[] =
{
		{"REG 192 Device state",					DT_Int16U, 1},
		{"REG 193 Fault reason",					DT_Int16U, 1},
		{"REG 194 Disable reason",					DT_Int16U, 1},
		{"REG 195 Warning",							DT_Int16U, 1},
		{"REG 196 Problem reason",					DT_Int16U, 1},
		{"CONTROL_Values_Counter",					DT_Int16U, 1},
		{"EP 1 Sampled gate voltage Vg (mV)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 2 Sampled gate current Ig (mA)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 3 Sampled direct voltage Vd (mV)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 4 Sampled direct current Id (mA)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 5 Control gate voltage Vg (mV)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 6 Control gate current Ig (mA)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 7 Control direct voltage Vd (mV)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 8 Control direct current Id (mA)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 9 Target gate voltage Vg (mV)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 10 Target gate current Ig (mA)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 11 Target direct voltage Vd (mV)",		DT_Int16U, VALUES_x_SIZE},
		{"EP 12 Target direct current Id (mA)",		DT_Int16U, VALUES_x_SIZE},
};
Int32U TablePointers[sizeof(StorageDescription) / sizeof(StorageDescription[0])] = {0};
const Int16U StorageSize = sizeof(StorageDescription) / sizeof(StorageDescription[0]);

