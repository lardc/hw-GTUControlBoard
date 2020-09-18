// -----------------------------------------
// System parameters
// ----------------------------------------
#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

// Include
#include <ZwBase.h>
#include <BoardConfig.h>

// CPU & System
//--------------------------------------------------------
#define CPU_PLL				10          // OSCCLK * PLL div 2 = CPUCLK: 20 * 10 / 2 = 100
#define CPU_CLKINDIV		0           // "div 2" in previous equation
#define SYS_HISPCP       	0x01   		// SYSCLKOUT / 2
#define SYS_LOSPCP       	0x02    	// SYSCLKOUT / 4
//--------------------------------------------------------

// Boot-loader
//--------------------------------------------------------
#define BOOT_LOADER_REQUEST	0xABCD
//--------------------------------------------------------

// Power control
//--------------------------------------------------------
#define SYS_PUMOD			ZW_POWER_ADC_CLK  | \
							ZW_POWER_SPIA_CLK | ZW_POWER_SPIB_CLK | ZW_POWER_SPID_CLK | \
							ZW_POWER_SCIB_CLK | ZW_POWER_CANA_CLK

#define SYS_WD_PRESCALER	0x07
//--------------------------------------------------------

// GPIO
//--------------------------------------------------------
// Input filters
#define GPIO_TSAMPLE		50		// T[sample_A] = (1/ 100MHz) * (2 * 50) = 1 uS
#define GPIO_NSAMPLE		6		// 6 samples: T = 1uS * 6 = 6 uS
//--------------------------------------------------------

// Flash
//--------------------------------------------------------
#define FLASH_FWAIT			3
#define FLASH_OTPWAIT		5
//--------------------------------------------------------

// TIMERs
//--------------------------------------------------------
#define CS_T0_FREQ			10000	// 10 KHz
#define CS_T2_FREQ			1000	// 1 KHz

#define TIMER0_PERIOD		(1000000L / CS_T0_FREQ)
#define TIMER2_PERIOD		(1000000L / CS_T2_FREQ)

#define DBG_FREQ			2		// 2 Hz
#define DBG_COUNTER_PERIOD	(1000 / (DBG_FREQ * 2))

#define BOARD_STARTUP_DELAY 2000
//--------------------------------------------------------

// DAC
//--------------------------------------------------------
#define DAC_BAUDRATE		25000000L	// SPI clock in Hz
#define DAC_PLR				TRUE		// CLK high in idle state (SPI MODE 4)		
#define DAC_PHASE			TRUE		// data output on falling edge (SPI MODE 4)
#define DAC_WRITE_DELAY_US	4
//--------------------------------------------------------

// SRAM and IOE
//--------------------------------------------------------
#define EPROM_BAUDRATE		1562500L	// SPI clock in Hz
#define EPROM_PLR			FALSE		// CLK low in idle state (SPI MODE 0)
#define EPROM_PHASE			TRUE		// data output on rising edge (SPI MODE 0)
//--------------------------------------------------------

// V REGISTER
//--------------------------------------------------------
#define VR_BAUDRATE			781250L		// SPI clock in Hz
#define VR_PLR				FALSE		// CLK low in idle state (SPI MODE 0)
#define VR_PHASE			TRUE		// data output on rising edge (SPI MODE 0)
//--------------------------------------------------------

// SCI-B
//--------------------------------------------------------
#define SCIB_BR				115200L		// UART baudrate = 115200 bps
#define SCIB_DB				8
#define SCIB_SB				FALSE
#define SCIB_PARITY			ZW_PAR_NONE
//--------------------------------------------------------

// CAN-A
//--------------------------------------------------------
#define CANA_BR				1000000L
#define CANA_BRP			9
#define CANA_TSEG1			6
#define CANA_TSEG2			1
#define CANA_SJW			1
//--------------------------------------------------------

// ADC
//--------------------------------------------------------
#define ADC_PRESCALER		6			// ADCLK = HSPCLK / (ADC_PRESCALER * 2)
#define ADC_CD2				FALSE		// Don't div ADC core / 2
#define ADC_SH				2			// S/H sample window
//--------------------------------------------------------

#endif // __SYSCONFIG_H
