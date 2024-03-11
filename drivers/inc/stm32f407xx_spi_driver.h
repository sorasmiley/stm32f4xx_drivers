#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

// Every driver header should contain this device-specific header file.
#include "stm32f407xx.h"

/****************************************************************************
 * Pin Configuration Settings
 ****************************************************************************/
typedef struct
{
	// SPI control register 1, MSTR (2nd bit field)
	// 1 is master, 0 is slave
	uint8_t SPI_DeviceMode;
	// SPI control register 1, BIDIMODE
	// 0 is full duplex (2-line, unidirectional) so we don't care about BIDIOE bit.
	// 1 is half duplex (1-line, bidirectional)

	// SPI control reigster 1, BIDIOE(BIDI output enable)
	// When you are using two devices in half-duplex, the modes must be reciprocal to each other.
	// If one device is TX, then another device must be RX.
	// So this bit controls output enable in bidirectional mode(half-duplex).
	// If bit is 0, then output is disabled so device will act as receiver.
	// If bit is 1, then device will act as transmitter.

	// For simplex communication, check 10th bit or RXONLY (receive only).
	// Only active when BIDIMODE is set to 0 or full duplex (transmit and receive).
	// If you want to use simplex communication with RXONLY mode, then you have to make bit as 1.
	uint8_t SPI_BusConfig;

	// Clock speed
	uint8_t SPI_SclkSpeed;

	// SPI control register 1, data frame format (11th bit)
	// 0 is data frame bit consists of 8 bits
	// 1 is data frame bit consists of 16 bits (16 bits of data will be transferred in one frame)
	uint8_t SPI_DFF;

	// SPI control register 1, 0 and 1 bits
	// CPOL: if bit is 0, clock is 0 when idle
	// CPHA: if bit is 0, 1st clock transition is data sampling edge
	//       if bit is 1, 2nd clock transition is data sampling edge
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;

	// SPI control register 1, bit 9
	// By default, it is 0. Software slave management is disabled.
	uint8_t SPI_SSM;
} SPI_PinConfig_t;

/****************************************************************************
 * Handle Structure
 ****************************************************************************/
typedef struct
{
	// pointer to hold the base address of GPIO peripheral
	SPI_RegDef_t *pSPIx; // This holds base address of SPIx(x=0,1,2)
	// pin configuration structure
	SPI_PinConfig_t SPIConfig;
} SPI_Handle_t;

/****************************************************************************
 * macros to initialize the handle variable SPI_PinConfig_t.
 ****************************************************************************/

/****************************************************************************
 * @SPI_DeviceMode
 * User has to refer to these macros in order to fill the device mode member element.
 ****************************************************************************/
#define SPI_DEVICE_MODE_MASTER		1
#define SPI_DEVICE_MODE_SLAVE		0
/****************************************************************************
 * @SPI_BusConfig
 *****************************************************************************/
#define SPI_BUS_CONFIG_FD				1 // full duplex
#define SPI_BUS_CONFIG_HD				2 // half duplex
//#define SPI_BUS_CONFIG_SIMPLEX_TXONLY	3 // simplex (TX only), nothing but full duplex only
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY	3 // simplex (RX only)
/****************************************************************************
 * @SPI_SclkSpeed
 *****************************************************************************/
#define SPI_SCLK_SPEED_DIV2				0
#define SPI_SCLK_SPEED_DIV4				1
#define SPI_SCLK_SPEED_DIV8				2
#define SPI_SCLK_SPEED_DIV16			3
#define SPI_SCLK_SPEED_DIV32			4
#define SPI_SCLK_SPEED_DIV64			5
#define SPI_SCLK_SPEED_DIV128			6
#define SPI_SCLK_SPEED_DIV256			7
/****************************************************************************
 * @SPI_DFF
 *****************************************************************************/
#define SPI_DFF_8BITS	0 // By default, 8 bits. So make it as 0.
#define SPI_DFF_16BITS	1
/****************************************************************************
 * @SPI_CPOL
 *****************************************************************************/
#define SPI_CPOL_HIGH	1
#define SPI_CPOL_LOW	0
/****************************************************************************
 * @SPI_CPHA
 *****************************************************************************/
#define SPI_CPHA_HIGH	1
#define SPI_CPHA_LOW	0
/****************************************************************************
 * @SPI_SSM
 *****************************************************************************/
#define SPI_SSM_EN 		1 // hardware management
#define SPI_SSM_DI		0 // by default, 0. 0 means software management is disabled.

/****************************************************************************
 * @SPI_SSM
 *****************************************************************************/
#define SPI_SSM_EN 		1 // hardware management
#define SPI_SSM_DI		0 // by default, 0. 0 means software management is disabled.

/****************************************************************************
 * SPI related status flags definitions
 *
 * We have many flags in the SPI_SR register.
 *****************************************************************************/
#define SPI_TXE_FLAG	( 1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG	( 1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG	( 1 << SPI_SR_BSY)

/****************************************************************************
 *							APIs supported by this driver
 * 		For more information about the APIs check the function definitions
 ****************************************************************************/

/***********************************************************************
 * Peripheral Clock setup
 ***********************************************************************/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

/***********************************************************************
 * Init and De-init
 ***********************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);
/****************************************************************************
 * Data Send and Receive
 *
 * 3 different types of data transmitting or receiving methodologies:
 * 1. Non-interrupt based API (blocking type)
 * 2. Interrupt based API (non-blocking type)
 * 3. Implement API (DMA based)
 ****************************************************************************/
// 1st parameter: pointer to base address of SPI peripheral
// 2nd parameter: user will give pointer to data (pointer to TX buffer)
// 3rd parameter: size of data transfer (how many bytes this API should send)
// The standard practice to define length as uint32_t or more than that.
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len); // RX buffer

/***********************************************************************
 * IRQ Configuration and ISR handling
 ***********************************************************************/
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

/***********************************************************************
 * Other Peripheral Control APIs
 ***********************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
