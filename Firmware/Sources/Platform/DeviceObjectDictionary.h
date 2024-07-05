// ----------------------------------------
// Logic controller
// ----------------------------------------

#ifndef __DEV_OBJ_DIC_H
#define __DEV_OBJ_DIC_H

#include "Global.h"

// ACTIONS
//
#define ACT_CLR_FAULT				3	// Clear fault (try switch state from FAULT to NONE)
#define ACT_CLR_WARNING				4	// Clear warning
//
#define ACT_DBG_DAC_CURR_GATE		11	// Write value to gate current DAC
#define ACT_DBG_DAC_CURR_DIRECT		12	// Write value to direct current DAC
#define ACT_DBG_KELVIN				13	// Write Kelvin state
#define ACT_DBG_DAC_VOLT_GATE		14	// Write value to gate voltage DAC
#define ACT_DBG_DAC_VOLT_DIRECT		15	// Write value to direct voltage DAC
#define ACT_DBG_OUTPUT_CTRL_GATE	16	// Enable/disable gate voltage output
#define ACT_DBG_OUTPUT_CTRL_DIRECT	17	// Enable/disable direct voltage output
#define ACT_DBG_SYNC_OSC			18	// Test output of oscilloscope synchronization
#define ACT_DBG_FAN					19	// Test FAN
#define ACT_DBG_GATE_PS_TUNE		20	// Test tune of gate power supply
#define ACT_DBG_EXT_OSC				21	// Read state OSC input from BVT
//
#define ACT_START_KELVIN			100	// Start Kelvin measurement
#define ACT_START_GATE				101	// Start gate measurement
#define ACT_START_IH				102	// Start holding current measurement
#define ACT_START_IL				103	// Start latching current measurement
#define ACT_START_RG				104 // Start gate resistance test
#define ACT_STOP_TEST				105 // Stop current test
#define ACT_START_VGNT				106	// Start Vgnt measurement

// Номера команд калибровки в режиме совместимости
#define ACT_CMP_START_CAL_IG		110 // Start Vg calibration
#define ACT_CMP_START_CAL_ID		111 // Start Ig calibration
#define ACT_CMP_START_CAL_VG		112 // Start Vg calibration
#define ACT_CMP_START_CAL_VD		113 // Start Vd calibration

// Номера команд калибровки в основном режиме
#define ACT_START_CAL_VG			110 // Start Vg calibration
#define ACT_START_CAL_IG			111 // Start Ig calibration
#define ACT_START_CAL_VD			112 // Start Vd calibration
#define ACT_START_CAL_ID			113 // Start Id calibration

#define ACT_SAVE_TO_ROM				200	// Save parameters to EEPROM module
#define ACT_RESTORE_FROM_ROM		201	// Restore parameters from EEPROM module
#define ACT_RESET_TO_DEFAULT		202	// Reset parameters to default values (only in controller memory)
#define ACT_LOCK_NV_AREA			203	// Lock modifications of parameters area
#define ACT_UNLOCK_NV_AREA			204	// Unlock modifications of parameters area (password-protected)
//
#define ACT_BOOT_LOADER_REQUEST		320	// Request reboot to bootloader
#define ACT_WRITE_LABEL1			321	// Записать первую метку: GTUControlBoard v.1.2 [BVT 5V Sync, Vg 5V]
#define ACT_READ_SYMBOL				330	// Выполнить чтение символа из памяти
#define ACT_SELECT_MEM_LABEL		331	// Переместить указатель считывания в область метки

#define ACT_FLASH_SAVE				332	// Flash save fault data
#define ACT_FLASH_ERASE				334	// Flash erase data sector

// REGISTERS
//
// Common settings
#define REG_COMM_VD_RISE_RATE		0	// Rise rate for direct voltage (in mV / ms)
#define REG_COMM_ID_LEAK_MAX		1	// Allowed direct current leakage (in mA)
#define REG_COMM_STAB_TIME			2	// Transient stabilizatioon time (in ms)
#define REG_COMM_STAB_ERROR			3	// Transient stabilizatioon error (in %)
#define REG_COMM_VG_MIN_INPUT		4	// Minimum allowed voltage input from Vg potential (in mV)
#define REG_COMM_CAL_STAB_ERROR		5	// Transient stabilizatioon error for calibration (in %)
// 6 - 8
#define REG_FAN_OFF_DELAY			9	// Fan turn off delay (in ms)
#define REG_PI_CTRL_VG_Kp			10	// Gate voltage PI Kp x1000
#define REG_PI_CTRL_VG_Ki			11	// Gate voltage PI Ki x1000
#define REG_PI_CTRL_IG_Kp			12	// Gate current PI Kp x1000
#define REG_PI_CTRL_IG_Ki			13	// Gate current PI Ki x1000
#define REG_PI_CTRL_VD_Kp			14	// Direct voltage PI Kp x1000
#define REG_PI_CTRL_VD_Ki			15	// Direct voltage PI Ki x1000
#define REG_PI_CTRL_ID_Kp			16	// Direct current PI Kp x1000
#define REG_PI_CTRL_ID_Ki			17	// Direct current PI Ki x1000

#define REG_ADC_VD_FINE_P2			18	// Fine coefficient for Vd quadratic correction P2 x1e6
#define REG_ADC_VD_FINE_P1			19  // Fine coefficient for Vd quadratic correction P1 x1000
#define REG_ADC_VD_FINE_P0			20	// Fine coefficient for Vd quadratic correction P0 (in mA)
#define REG_ADC_VD_CONV_K			21	// Conversion coefficient K for Vd x1000
#define REG_ADC_VD_CONV_B			22	// Conversion offset for Vd (in mV)

#define REG_ADC_ID_FINE_P2			23	// Fine coefficient for Id quadratic correction P2 x1e6
#define REG_ADC_ID_FINE_P1			24  // Fine coefficient for Id quadratic correction P1 x1000
#define REG_ADC_ID_FINE_P0			25	// Fine coefficient for Id quadratic correction P0 (in mA)
#define REG_ADC_ID_CONV_K			26	// Conversion coefficient K for Id x1000
#define REG_ADC_ID_CONV_B			27	// Conversion offset for Id (in mA)

#define REG_ADC_VG_FINE_P2			28	// Fine coefficient for Vg quadratic correction P2 x1e6
#define REG_ADC_VG_FINE_P1			29	// Fine coefficient for Vg quadratic correction P1 x1000
#define REG_ADC_VG_FINE_P0			30	// Fine coefficient for Vg quadratic correction P0 (in mV)
#define REG_ADC_VG_CONV_K			31	// Conversion coefficient K for Vg x1000
#define REG_ADC_VG_CONV_B			32	// Conversion offset for Vg (in mV)

#define REG_ADC_IG_FINE_P2			33	// Fine coefficient for Ig quadratic correction P2 x1e6
#define REG_ADC_IG_FINE_P1			34	// Fine coefficient for Ig quadratic correction P1 x1000
#define REG_ADC_IG_FINE_P0			35	// Fine coefficient for Ig quadratic correction P0 (in mA)
#define REG_ADC_IG_CONV_K			36	// Conversion coefficient K for Ig x1000
#define REG_ADC_IG_CONV_B			37	// Conversion offset for Ig (in mA)

#define REG_DAC_VD_CONV_K			38	// Conversion coefficient K for direct voltage x1000
#define REG_DAC_VD_CONV_B			39	// Conversion offset for direct voltage (in mV)

#define REG_DAC_ID_FINE_P2			40	// Fine coefficient for Id quadratic correction P2 x1e6
#define REG_DAC_ID_FINE_P1			41	// Fine coefficient for Id quadratic correction P1 x1000
#define REG_DAC_ID_FINE_P0			42	// Fine coefficient for Id quadratic correction P0 (in mV)
#define REG_DAC_ID_CONV_K			43	// Conversion coefficient K for direct current x1000
#define REG_DAC_ID_CONV_B			44	// Conversion offset for direct current (in mA)

#define REG_DAC_VG_CONV_K			45	// Conversion coefficient K for gate voltage x1000
#define REG_DAC_VG_CONV_B			46	// Conversion offset for gate voltage (in mV)

#define REG_DAC_IG_FINE_P2			47	// Fine coefficient for Ig quadratic correction P2 x1e6
#define REG_DAC_IG_FINE_P1			48	// Fine coefficient for Ig quadratic correction P1 x1000
#define REG_DAC_IG_FINE_P0			49	// Fine coefficient for Ig quadratic correction P0 (in mV)
#define REG_DAC_IG_CONV_K			50	// Conversion coefficient K for gate current x1000
#define REG_DAC_IG_CONV_B			51	// Conversion offset for gate current (in mA)
// 52 - 54
#define REG_KELVIN_PROBE_TIME		55	// Time for probing (in ms)
// 56 - 59
#define REG_GATE_VG_RATE			60	// Vg rise rate for gate measurement (in mV / ms)
#define REG_VGNT_VG_RATE			61	// Vg rise rate for Vgnt measurement (in mV / s)
#define REG_VGNT_CONF_VG_RATE		62	// Vg rise rate for Vgnt confirmation (in mV / s)
#define REG_VGNT_CONF_TIME			63	// Vgnt confirmation time (in ms)
#define REG_VGNT_VG_TIME_ERR_ZONE	64	// Time from the start to activly monitor Vg sensing problems (in ms)
#define REG_VGNT_MEAS_POINT_OFFSET	65	// Measurement point offset from SYNC event (in ticks)
// 66 - 69
#define REG_HOLD_END_CURRENT		70	// Minimum available current for Ih (in mA)
#define REG_HOLD_CURRENT_FALL_RATE	71	// Current rate (mA / ms)
#define REG_HOLD_VCLOSE_LEVEL		72	// Порог напряжения Vd для определения запирания прибора (в В)
#define REG_HOLD_TRIG_WAIT_GOST		73	// Время ожидания отпирания прибора блоком стат. потерь по ГОСТ (в мс)
#define REG_HOLD_BACK_SHIFT			74	// Сдвиг назад при измерении Ih (в тиках)
// 75 - 79
#define REG_LATCH_RATE_LARGE		80	// Large step of increasing Id for latching test (in mA / ms)
#define REG_LATCH_RATE_SMALL		81	// Small step of increasing Id for latching test (in mA / ms)
#define REG_LATCH_START_CURRENT		82	// Test start current (in mA)
#define REG_LATCH_STAB_TIME			83	// Transient stabilizatioon time (in ms)
// 84 - 89
#define REG_RG_CURRENT				90	// Test current (in mA)
#define REG_RG_CURRENT_RATE			91	// Current rise rate (in mA / ms)
// 92 - 99
#define REG_CAL_V_RISE_RATE			100	// Calibration voltage rise rate (in mV / ms)
#define REG_CAL_I_RISE_RATE			101	// Calibration current rise rate (in mA / ms)
// 102 - 104
#define REG_DAC_LOW_IG_FINE_P2		105	// Fine coefficient for low Ig quadratic correction P2 x1e6
#define REG_DAC_LOW_IG_FINE_P1		106	// Fine coefficient for low Ig quadratic correction P1 x1000
#define REG_DAC_LOW_IG_FINE_P0		107	// Fine coefficient for low Ig quadratic correction P0 (in mA)
#define REG_DAC_LOW_IG_CONV_K		108	// Conversion coefficient K for low gate current x1000
#define REG_DAC_LOW_IG_CONV_B		109	// Conversion offset for low gate current (in mA)
#define REG_LOW_IG_LIMIT			110 // Верхняя граница для младшего диапазона (в мА)
// 111 - 114
#define REG_ADC_LOW_IG_FINE_P2		115	// Fine coefficient for low Ig quadratic correction P2 x1e6
#define REG_ADC_LOW_IG_FINE_P1		116	// Fine coefficient for low Ig quadratic correction P1 x1000
#define REG_ADC_LOW_IG_FINE_P0		117	// Fine coefficient for low Ig quadratic correction P0 (in mA)
#define REG_ADC_LOW_IG_CONV_K		118	// Conversion coefficient K for low gate current x1000
#define REG_ADC_LOW_IG_CONV_B		119	// Conversion offset for low gate current (in mA)

#define REG_OLD_GTU_COMPATIBLE		120	// Режим совместимости с блоками GTU на базе ЭМ GateTester
// ----------------------------------------
//
#define REG_GATE_VGT_PURE			128	// Sample VGT after terminating main current
#define REG_HOLD_USE_STRIKE			129	// Strike pulse before holding current (for compatibility only)
#define REG_HOLD_WITH_SL			130	// Measure holding current with static losses
//
// В режиме совместимости запись в регистры 131 - 134 заблокирована
#define REG_V_DIRECT_LIMIT			131	// Direct voltage limit, Vd (in mV)
#define REG_I_DIRECT_LIMIT			132	// Direct current limit, Id (in mA)
#define REG_V_GATE_LIMIT			133	// Gate voltage limit, Vg (in mV)
#define REG_I_GATE_LIMIT			134	// Gate current limit, Ig (in mA)
#define REG_VGNT_CONF_MODE			135 // Confirmation mode for Vgnt measurement

#define REG_CAL_CURRENT				140	// В режиме совместимости - ток калибровки, мА

#define REG_SCOPE_TYPE_1			150	// Scope type 1
#define REG_SCOPE_TYPE_2			151	// Scope type 2
#define REG_SCOPE_RATE				152	// Scope rate divisor
//
#define REG_DBG						160	// General purpose debug register

#define REG_DBG_STATE_EXT_OSC		165	// State external OSC
//
#define REG_PWD_1					180	// Unlock password location 1
#define REG_PWD_2					181	// Unlock password location 2
#define REG_PWD_3					182	// Unlock password location 3
#define REG_PWD_4					183	// Unlock password location 4
//
// ----------------------------------------
//
#define REG_DEV_STATE				192	// Device state
#define REG_FAULT_REASON			193	// Fault reason in the case DeviceState -> FAULT
#define REG_DISABLE_REASON			194	// Fault reason in the case DeviceState -> DISABLED
#define REG_WARNING					195	// Warning if present
#define REG_PROBLEM					196	// Problem reason
#define REG_TEST_FINISHED			197	// Indicates that test is complete
#define REG_RESULT_KELVIN			198	// Kelvin aggregated result
#define REG_RESULT_IGT				199	// IGT (in mA)
#define REG_RESULT_VGT				200	// VGT (in mV)
#define REG_RESULT_IH				201	// IH  (in mA)
#define REG_RESULT_IL				202	// IL  (in mA)
#define REG_RESULT_RG				203	// R   (in Ohm * 10)
#define REG_RESULT_CAL				204	// Calibration result
#define REG_RESULT_CAL_V			205	// В режиме совместимости - напряжение Vgt во время калибровки, мВ
#define REG_RESULT_VGNT				205	// Vgnt (in mV)
#define REG_RESULT_IGNT				206	// Ignt (in mA)
//
#define REG_KELVIN_1_2				211	// MUST BE 1
#define REG_KELVIN_4_1				212	// MUST BE 0
#define REG_KELVIN_1_4				213	// MUST BE 0
#define REG_KELVIN_3_2				214	// MUST BE 1
//
#define REG_PROCESS_COUNTER			215	// Operation tick counter
#define REG_EP_LAST_POINTER			216	// Pointer to last element in EP cycle arrays
//
#define REG_CAN_BUSOFF_COUNTER		220 // Counter of bus-off states
#define REG_CAN_STATUS_REG			221	// CAN status register (32 bit)
#define REG_CAN_STATUS_REG_32		222
#define REG_CAN_DIAG_TEC			223	// CAN TEC
#define REG_CAN_DIAG_REC			224	// CAN REC
//
#define REG_RESULT_IH_UA			231	// IH fraction part (in uA)
#define REG_RESULT_IGT_UA			232	// IGT fraction part (in uA)
#define REG_RESULT_CAL_FRAC			233	// Calibration result fractional part
#define REG_RESULT_CAL_V_FRAC		234	// В режиме совместимости - дробная часть напряжения Vgt во время калибровки
//
#define REG_QUADRATIC_CORR			254	// Use quadratic correction for block
//
// ----------------------------------------
//
#define REG_FWINFO_SLAVE_NID		256	// Device CAN slave node ID
#define REG_FWINFO_MASTER_NID		257	// Device CAN master node ID (if presented)
// 258 - 259
#define REG_FWINFO_STR_LEN			260	// Length of the information string record
#define REG_FWINFO_STR_BEGIN		261	// Begining of the information string record
//
#define REG_MEM_SYMBOL				299	// Считанный по адресу памяти символ

#define REG_FLASH_WRITE_DATA		184	// Flash temporary data register
#define REG_FLASH_WRITE_TYPE		185	// Flash data type

// SCOPE TYPE
// Выбор сохраняемой осциллограммы в режиме совместимости
#define SCOPE_TYPE_NONE				0	// Deactivate this scope
#define SCOPE_TYPE_I				1	// Log I values
#define SCOPE_TYPE_V				2	// Log V values
#define SCOPE_TYPE_TARGET_I			3	// Log target I values
#define SCOPE_TYPE_CONTROL			4	// Log control signal's values

// ENDPOINTS
// В основном режиме
#define EP16_Data_Vg				1	// Sampled gate voltage Vg (mV)
#define EP16_Data_Ig				2	// Sampled gate current Ig (mA)
#define EP16_Data_Vd				3	// Sampled direct voltage Vd (mV)
#define EP16_Data_Id				4	// Sampled direct current Id (mA)
//
#define EP16_Control_Vg				5
#define EP16_Control_Ig				6
#define EP16_Control_Vd				7
#define EP16_Control_Id				8
//
#define EP16_Target_Vg				9	// Target gate voltage Vg (mV)
#define EP16_Target_Ig				10	// Target gate current Ig (mA)
#define EP16_Target_Vd				11	// Target direct voltage Vd (mV)
#define EP16_Target_Id				12	// Target direct current Id (mA)

// ENDPOINTS
// В режиме совместимости
#define EP16_Scope1					1
#define EP16_Scope2					2

// OPERATION RESULTS
//
#define OPRESULT_NONE				0	// No information or not finished
#define OPRESULT_OK					1	// Operation was successful
#define OPRESULT_FAIL				2	// Operation failed

// PROBLEM CODES
//
#define PROBLEM_NONE				0
#define PROBLEM_OPERATION_STOPPED	1	// Process was terminated by user
#define PROBLEM_VD_SET_ERR			2	// Vd stabilization error
#define PROBLEM_ID_SET_ERR			3	// Id stabilization error
#define PROBLEM_VG_SET_ERR			4	// Vg stabilization error
#define PROBLEM_IG_SET_ERR			5	// Ig stabilization error
#define PROBLEM_ID_LEAK				6	// Unexpected direct current
#define PROBLEM_DUT_NO_TRIG			7	// DUT not switched on
#define PROBLEM_DUT_NO_CLOSE		8	// DUT not switched off
#define PROBLEM_DUT_NO_LATCHING		9	// DUT not latched
#define PROBLEM_DUT_NO_VG_SENSING	10	// No potential signal from DUT Vg
#define PROBLEM_VGNT_CONF_TRIG		11	// DUT trigged during Vgnt confirmation

// FAULT CODES
//
#define FAULT_NONE					0

// WARNING CODES
//
#define WARNING_NONE				0
#define WARNING_WATCHDOG_RESET		1001	// System has been reseted by WD

// DISABLE CODES
//
#define DISABLE_NONE				0
#define DISABLE_BAD_CLOCK			1001	// Problem with main oscillator

// USER ERROR CODES
//
#define ERR_NONE					0	// No error
#define ERR_CONFIGURATION_LOCKED	1	// Device is locked for writing
#define ERR_OPERATION_BLOCKED		2	// Operation can't be done due to current device state
#define ERR_DEVICE_NOT_READY		3	// Device isn't ready to switch state
#define ERR_WRONG_PWD				4	// Wrong password - unlock failed


#endif // __DEV_OBJ_DIC_H
