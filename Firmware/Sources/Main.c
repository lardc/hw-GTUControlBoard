// ----------------------------------------
// Program entry point
// ----------------------------------------

// Include
#include <stdinc.h>
//
#include "ZwDSP.h"
#include "ZbBoard.h"
//
#include "SysConfig.h"
//
#include "Controller.h"
#include "Flash.h"


// FORWARD FUNCTIONS
// ----------------------------------------
Boolean InitializeCPU();
void InitializeTimers();
void InitializeADC();
void InitializeSCI();
void InitializeSPI();
void InitializeCAN();
void InitializeBoard();
void InitializeController(Boolean GoodClock);
// ----------------------------------------


// FORWARD ISRs
// ----------------------------------------
// CPU Timer 0 ISR
ISRCALL Timer0_ISR();
// CPU Timer 2 ISR
ISRCALL Timer2_ISR();
// CAN Line 0 ISR
ISRCALL CAN0_ISR();
// ADC SEQ1 ISR
ISRCALL SEQ1_ISR();
// ILLEGAL ISR
ISRCALL IllegalInstruction_ISR();
// ----------------------------------------


// FUNCTIONS
// ----------------------------------------
void main()
{
	Boolean clockInitResult;

	// Boot process
	clockInitResult = InitializeCPU();
	FLASH_Init();

	// Only if good clocking was established
	if(clockInitResult)
	{
		InitializeTimers();
		InitializeADC();
		InitializeSPI();
		InitializeCAN();
		InitializeBoard();
	}

	// Try initialize SCI in spite of result of clock initialization
	InitializeSCI();

	// Setup ISRs
	BEGIN_ISR_MAP
		ADD_ISR(TINT0, Timer0_ISR);
		ADD_ISR(TINT2, Timer2_ISR);
		ADD_ISR(ECAN0INTA, CAN0_ISR);
		ADD_ISR(SEQ1INT, SEQ1_ISR);
	END_ISR_MAP

	// Initialize controller logic
	InitializeController(clockInitResult);
	
	// Only if good clocking was established
	if(clockInitResult)
	{
		// Enable interrupts
		EINT;
		ERTM;

		// Set watch-dog as WDRST
		ZwSystem_SelectDogFunc(FALSE);
		// Enable watch-dog
		ZwSystem_EnableDog(SYS_WD_PRESCALER);
	
		// Start system ticks timer
		ZwTimer_StartT2();
	}
	
	// Low-priority services
	while(TRUE)
		CONTROL_Idle();
}
// ----------------------------------------

Boolean InitializeCPU()
{
    Boolean clockInitResult;

	// Init clock and peripherals
    clockInitResult = ZwSystem_Init(CPU_PLL, CPU_CLKINDIV, SYS_LOSPCP, SYS_HISPCP, SYS_PUMOD);

    if(clockInitResult)
    {
		// Do default GPIO configuration
		ZwGPIO_Init(GPIO_TSAMPLE, GPIO_TSAMPLE, GPIO_TSAMPLE, GPIO_TSAMPLE, GPIO_TSAMPLE);

		// Initialize PIE and vectors
		ZwPIE_Init();
		ZwPIE_Prepare();
    }
   	
	// Config flash
	ZW_FLASH_CODE_SHADOW;
	ZW_FLASH_CONST_SHADOW;
	ZW_FLASH_MATH_SHADOW;
	ZW_FLASH_OPTIMIZE(FLASH_FWAIT, FLASH_OTPWAIT);

   	return clockInitResult;
}
// ----------------------------------------

void InitializeTimers()
{
    ZwTimer_InitT0();
	ZwTimer_SetT0(TIMER0_PERIOD);
	ZwTimer_EnableInterruptsT0(TRUE);

    ZwTimer_InitT2();
	ZwTimer_SetT2(TIMER2_PERIOD);
	ZwTimer_EnableInterruptsT2(TRUE);
}
// ----------------------------------------

void InitializeADC()
{
	const Int16U ADC_Channels[16] = { AIN_VG, AIN_IG, AIN_VD, AIN_ID, AIN_VG, AIN_IG, AIN_VD, AIN_ID,
									  AIN_VG, AIN_IG, AIN_VD, AIN_ID, AIN_VG, AIN_IG, AIN_VD, AIN_ID };

	ZwADC_Init(ADC_PRESCALER, ADC_CD2, ADC_SH);
	ZwADC_ConfigInterrupts(TRUE, FALSE);

	// Enable interrupts on peripheral and CPU levels
	ZwADC_EnableInterrupts(TRUE, FALSE);
	ZwADC_EnableInterruptsGlobal(TRUE);

	// Config sampling channels
	ZwADC_ConfigureSequentialCascaded(16, ADC_Channels);
}
// ----------------------------------------

void InitializeSCI()
{
	ZwSCIb_Init(SCIB_BR, SCIB_DB, SCIB_PARITY, SCIB_SB, FALSE);
	ZwSCIb_InitFIFO(16, 0);
	ZwSCIb_EnableInterrupts(FALSE, FALSE);

	ZwSCI_EnableInterruptsGlobal(FALSE);
}
// ----------------------------------------

void InitializeSPI()
{
	// Common (ABCD)
	ZwSPI_EnableInterruptsGlobal(FALSE);
}
// ----------------------------------------

void InitializeCAN()
{
	// Init CAN
	ZwCANa_Init(CANA_BR, CANA_BRP, CANA_TSEG1, CANA_TSEG2, CANA_SJW);

	// Register system handler
	ZwCANa_RegisterSysEventHandler(&CONTROL_NotifyCANFault);

    // Allow interrupts for CAN
    ZwCANa_InitInterrupts(TRUE);
    ZwCANa_EnableInterrupts(TRUE);
}
// ----------------------------------------

void InitializeBoard()
{
	ZbGPIO_Init();
	ZbDAC_Init();
   	ZbMEM_Init();
   	ZbVRegister_Init();
}
// ----------------------------------------

void InitializeController(Boolean GoodClock)
{
	// Init controllers and logic
	CONTROL_Init(!GoodClock);
}
// ----------------------------------------


// ISRs
// ----------------------------------------
#ifdef BOOT_FROM_FLASH
	#pragma CODE_SECTION(Timer0_ISR, "ramfuncs");
	#pragma CODE_SECTION(Timer2_ISR, "ramfuncs");
	#pragma CODE_SECTION(CAN0_ISR, "ramfuncs");
	#pragma CODE_SECTION(SEQ1_ISR, "ramfuncs");
	#pragma CODE_SECTION(IllegalInstruction_ISR, "ramfuncs");
#endif
//
#pragma INTERRUPT(Timer0_ISR, HPI);
#pragma INTERRUPT(SEQ1_ISR, HPI);

// timer 0 ISR
ISRCALL Timer0_ISR(void)
{
	// Invoke ADC sampling
	ZwADC_StartSEQ1();
	
	// allow other interrupts from group 1
	TIMER0_ISR_DONE;
}
// ----------------------------------------

// timer 2 ISR
ISRCALL Timer2_ISR(void)
{
	static Int16U dbgCounter = 0;

	// Update time
	++CONTROL_TimeCounter;

	// Service watch-dogs
	if (CONTROL_BootLoaderRequest != BOOT_LOADER_REQUEST)
		ZwSystem_ServiceDog();

	++dbgCounter;
	if(dbgCounter == DBG_COUNTER_PERIOD)
	{
		ZbGPIO_ToggleLED2();
		dbgCounter = 0;
	}

	FAN_Control();

	// no PIE
	TIMER2_ISR_DONE;
}
// ----------------------------------------

// ADC SEQ1 ISR
ISRCALL SEQ1_ISR(void)
{
	// Handle interrupt
	ZwADC_ProcessInterruptSEQ1();
	// Dispatch results

	CONTROL_UpdateHigh();

	// allow other interrupts from group 1
	ADC_ISR_DONE;
}
// ----------------------------------------

// Line 0 ISR
ISRCALL CAN0_ISR(void)
{
    // handle CAN system events
	ZwCANa_DispatchSysEvent();
	// allow other interrupts from group 9
	CAN_ISR_DONE;
}
// ----------------------------------------

// ILLEGAL ISR
ISRCALL IllegalInstruction_ISR(void)
{
	// Disable interrupts
	DINT;

	// Reset system using WD
	ZwSystem_ForceDog();
}
// ----------------------------------------


