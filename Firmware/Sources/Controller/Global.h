// -----------------------------------------
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
//--------------------------------------------------------

// Regulator parameters
//--------------------------------------------------------
#define REGLTR_VG_SAT					_IQ(20000.0f)	// ��������� ������ ���������� Vg (� ��)
#define REGLTR_IG_SAT					_IQ(1200.0f)	// ��������� ������ ���������� Ig (� ��)
#define REGLTR_VD_SAT					_IQ(20000.0f)	// ��������� ������ ���������� Vd (� ��)
#define REGLTR_ID_SAT					_IQ(1200.0f)	// ��������� ������ ���������� Id (� ��)
#define REGLTR_ERROR_I_SAT_H			_IQ(20000.0f)	// �������� ��������� ��� ������������ ������������ ����������
//--------------------------------------------------------

#endif // __GLOBAL_H
