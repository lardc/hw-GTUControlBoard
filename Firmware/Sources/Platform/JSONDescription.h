#ifndef __JSON_DESCRIPTION_H
#define __JSON_DESCRIPTION_H

#include "Global.h"

#define JSON_POINTERS_SIZE 32

extern Int16U Vgt1Max;
extern Int16U Igt1Max, Igt2Max;
extern Int16U Ih1Min, Ih1Max;
extern Int16U IL1Max;

extern Boolean Vgt1Active, Igt1Active;

extern Int16U JSONPointers[JSON_POINTERS_SIZE];

void JSON_AssignPointer(Int16U Index, Int32U Pointer);

#endif // __JSON_DESCRIPTION_H
