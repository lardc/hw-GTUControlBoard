// ----------------------------------------
// Board parameters
// ----------------------------------------

#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

// Include
#include <ZwBase.h>

// Program build mode
#define BOOT_FROM_FLASH		// normal mode

// Board options
#define CPU_FRQ_MHZ			100			// CPU frequency = 100MHz
#define CPU_FRQ				(CPU_FRQ_MHZ * 1000000L)
#define SYS_HSP_FREQ		(CPU_FRQ / 2) // High-speed bus frequency
#define SYS_LSP_FREQ		(CPU_FRQ / 4) // Low-speed bus frequency
//
#define ZW_PWM_DUTY_BASE	1024

// Peripheral options
#define HWUSE_SPI_A
#define HWUSE_SPI_B
#define HWUSE_SPI_D
#define HWUSE_SCI_B

// IO placement
#define SPI_A_QSEL		    	GPAQSEL2
#define SPI_A_MUX				GPAMUX2
#define SPI_A_SIMO				GPIO16
#define SPI_A_SOMI				GPIO17
#define SPI_A_CLK				GPIO18
#define SPI_A_CS				GPIO19
//
#define SPI_B_QSEL				GPAQSEL2
#define SPI_B_MUX				GPAMUX2
#define SPI_B_SIMO				GPIO24
#define SPI_B_SOMI				GPIO25
#define SPI_B_CLK				GPIO26
#define SPI_B_CS				GPIO27
//
#define SPI_D_QSEL				GPAQSEL1
#define SPI_D_MUX				GPAMUX1
#define SPI_D_SIMO				GPIO1
#define SPI_D_SOMI				GPIO3
#define SPI_D_CLK				GPIO5
#define SPI_D_CS				GPIO7
//
#define SCI_B_QSEL				GPAQSEL1
#define SCI_B_MUX				GPAMUX1
#define SCI_B_TX				GPIO9
#define SCI_B_RX				GPIO11
#define SCI_B_MUX_SELECTOR		2
//
#define PIN_PDAC_LDAC			12
#define PIN_VRF_SENSE			13
#define PIN_LED_2				6
#define PIN_GDAC_LDAC			15
#define PIN_DAC_W2				21
#define PIN_KREG_OE				17
#define PIN_LED_1				14
#define PIN_EXT_SYNC			25
#define PIN_GATE_ENABLE_LGC		28
#define PIN_DAC_W1				8
#define PIN_MP_ENABLE_LGC		32
#define PIN_SPI_W1				20
#define PIN_GATE_PS_TUNE_EN_LGC	23
#define PIN_SYNC_OSC			4
#define PIN_FAN					0


// ADC placement in sequential mode
#define AIN_VG				0x1
#define AIN_IG				0x2
#define AIN_VD				0x9
#define AIN_ID				0x8

#endif // __BOARD_CONFIG_H
