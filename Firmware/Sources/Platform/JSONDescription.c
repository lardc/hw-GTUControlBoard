#include "JSONDescription.h"
#include "FormatOutputJSON.h"

const char TemplateJSON[] = "[{\n"
"	'param' : 'Vgt',\n"
"	'type' : 'measure',\n"
"	'ranges': [\n"
"		{\n"
"			'rangeId' : '1',\n"
"			'unitsMultiply' : '1'\n"
"			'active' : '$',\n"
"			'min': '100',\n"
"			'max': '$'\n"
"		},\n"
"		{\n"
"			'rangeId' : '2',\n"
"			'unitsMultiply' : '1',\n"
"			'active' : '1',\n"
"			'min': '$',\n"
"			'max': '5000'\n"
"		}]\n"
"},\n"
"{\n"
"	'param' : 'Igt',\n"
"	'type' : 'set',\n"
"	'ranges': [\n"
"		{\n"
"			'rangeId' : '3',\n"
"			'unitsMultiply' : '1',\n"
"			'active' : '$',\n"
"			'min': '10',\n"
"			'max': '$'\n"
"		},\n"
"		{\n"
"			'rangeId' : '4',\n"
"			'unitsMultiply' : '1',\n"
"			'active' : '1',\n"
"			'min': '$',\n"
"			'max': '$'\n"
"		}]\n"
"},\n"
"{\n"
"	'param' : 'Ih',\n"
"	'type' : 'set',\n"
"	'ranges': [\n"
"		{\n"
"			'rangeId' : '5',\n"
"			'unitsMultiply' : '1',\n"
"			'active' : '1',\n"
"			'min': '$',\n"
"			'max': '$'\n"
"		}]\n"
"},\n"
"{\n"
"	'param' : 'IL',\n"
"	'type' : 'set',\n"
"	'ranges': [\n"
"		{\n"
"			'rangeId' : '6',\n"
"			'unitsMultiply' : '1',\n"
"			'active' : '1',\n"
"			'min': '50',\n"
"			'max': '$'\n"
"		}]\n"
"},\n"
"{\n"
"	'param' : 'Rg',\n"
"	'type' : 'measure',\n"
"	'ranges': [\n"
"		{\n"
"			'rangeId' : '7',\n"
"			'unitsMultiply' : '1',\n"
"			'active' : '1',\n"
"			'min': '2',\n"
"			'max': '30'\n"
"		}]\n"
"}]";

Int16U Vgt1Max;
Int16U Igt1Max, Igt2Max;
Int16U Ih1Min, Ih1Max;
Int16U IL1Max;

Boolean Vgt1Active, Igt1Active;

Int16U JSONPointers[JSON_POINTERS_SIZE] = {0};

void JSON_AssignPointer(Int16U Index, Int32U Pointer)
{
	if (Index < JSON_POINTERS_SIZE)
		JSONPointers[Index] = Pointer;
}

