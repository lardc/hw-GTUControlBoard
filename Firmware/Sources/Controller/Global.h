﻿// ----------------------------------------
// Global definitions
// ----------------------------------------

#ifndef __GLOBAL_H
#define __GLOBAL_H

// Include
#include "stdinc.h"

// Global parameters
//--------------------------------------------------------
// Password to unlock non-volatile area for write
#define ENABLE_LOCKING					FALSE
#define UNLOCK_PWD_1					1
#define UNLOCK_PWD_2					1
#define UNLOCK_PWD_3					1
#define UNLOCK_PWD_4					1
//
#define	SCCI_TIMEOUT_TICKS				1000
#define DT_EPROM_ADDRESS				0
#define EP_COUNT						13
#define VALUES_x_SIZE					800
#define VALUES_EXT_INFO_SIZE			300
//--------------------------------------------------------

// Regulator parameters
//--------------------------------------------------------
#define REGLTR_I_MAX					1100
// Защитные уровни насыщения переменных регулятора
// должны быть больше максимальных допустимых выходных значений.
#define REGLTR_Ix_SAT			_IQ(2 * REGLTR_I_MAX)	// Выхода регулятора тока (в мА)
#define REGLTR_Vx_SAT					_IQ(20000)		// Выход регулятора напряжения (в мВ)
#define REGLTR_ERROR_I_SAT_H			_IQ(20000)		// Интегральная составляющая
//--------------------------------------------------------

// Specific parameters
//--------------------------------------------------------
#define VGNT_RING_BUFFER_SIZE			50				// Размер кольцевого буфера Vgnt для сдвига точки измерения
#define IH_RING_BUFFER_SIZE				50				// Размер кольцевого буфера Ih для сдвига точки измерения
#define RG_MAX_VAL						_IQ(40)			// Максимально допустимое сопротивление, Ом
#define VG_1_MAX_VAL					_IQ(500)		//
#define IG_1_MAX_VAL					_IQ(50)			//
#define IG_2_MAX_VAL					_IQ(600)		//
#define IH_MAX_VAL						_IQ(600)		//
#define IL_MAX_VAL						_IQ(600)		//
//--------------------------------------------------------

#endif // __GLOBAL_H
