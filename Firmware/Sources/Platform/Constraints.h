// -----------------------------------------
// Constraints for tunable parameters
// ----------------------------------------

#ifndef __CONSTRAINTS_H
#define __CONSTRAINTS_H

// Include
#include "stdinc.h"
//
#include "DataTable.h"
#include "Global.h"

// Types
//
typedef struct __TableItemConstraint
{
	Int16U Min;
	Int16U Max;
	Int16U Default;
} TableItemConstraint;


// Restrictions
//
// in mV
#define VD_LIM_MIN					1500
#define VD_LIM_MAX					12500
#define VD_LIM_DEF					12000

// in mA
#define ID_LIM_MIN					25
#define ID_LIM_MAX					1100
#define ID_LIM_DEF					1000

// in mV
#define VG_LIM_MIN					100
#define VG_LIM_MAX					12500
#define VG_LIM_DEF					5000

// in mA
#define IG_LIM_MIN					25
#define IG_LIM_MAX					1100
#define IG_LIM_DEF					1000


// in mV/ms or mA/ms
#define RISE_FALL_RATE_MIN			1
#define RISE_FALL_RATE_MAX			1000
#define RISE_FALL_RATE_DEF			10

// in mA
#define ID_LEAK_MIN					1
#define ID_LEAK_MAX					50
#define ID_LEAK_DEF					10

// in ms
#define TIME_MIN					1
#define TIME_MAX					100
#define TIME_DEF					10

// in %
#define PERCENT_MIN					1
#define PERCENT_MAX					100
#define PERCENT_DEF_STAB_ERR		5
//
#define PROPORTIONAL_MIN			1
#define PROPORTIONAL_DEF			1000

// in mA
#define HOLD_END_MIN				1
#define HOLD_END_MAX				100
#define HOLD_END_DEF				20

// in mA
#define LATCH_RATE_SMALL_DEF		1

// in mA
#define LATCH_START_CURR_MIN		30
#define LATCH_START_CURR_MAX		200
#define LATCH_START_CURR_DEF		50

// in mA
#define RG_CURRENT_MIN				1
#define RG_CURRENT_MAX				200
#define RG_CURRENT_DEF				50


// Variables
//
extern const TableItemConstraint NVConstraint[DATA_TABLE_NV_SIZE];
extern const TableItemConstraint VConstraint[DATA_TABLE_WP_START - DATA_TABLE_WR_START];


#endif // __CONSTRAINTS_H
