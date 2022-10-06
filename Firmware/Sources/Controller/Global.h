// ----------------------------------------
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
#define EP_COUNT						12
#define VALUES_x_SIZE					1000
//
#define CAL_COMPATIBILITY				FALSE			// Режим совместемости калибровки
//--------------------------------------------------------

// Regulator parameters
//--------------------------------------------------------
#define REGLTR_VG_SAT					_IQ(20000.0f)	// Насыщение выхода регулятора Vg (в мВ)
#define REGLTR_IG_SAT					_IQ(1200.0f)	// Насыщение выхода регулятора Ig (в мА)
#define REGLTR_VD_SAT					_IQ(20000.0f)	// Насыщение выхода регулятора Vd (в мВ)
#define REGLTR_ID_SAT					_IQ(1200.0f)	// Насыщение выхода регулятора Id (в мА)
#define REGLTR_ERROR_I_SAT_H			_IQ(20000.0f)	// Значение насыщения для интегральной составляющей регулятора
//--------------------------------------------------------

// Specific parameters
//--------------------------------------------------------
#define VGNT_RING_BUFFER_SIZE			50				// Размер кольцевого буфера Vgnt для сдвига точки измерения
//--------------------------------------------------------

#endif // __GLOBAL_H
