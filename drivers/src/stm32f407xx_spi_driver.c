// In driver.c, you have to include respective peripheral's driver file.
#include "stm32f407xx_spi_driver.h"
/**********************************************************************
 * Peripheral Clock setup
 * (Peripheral Control API)
 * ********************************************************************
 * @fn			- SPI_PeriClockControl
 *
 * @brief		- This function enables or disables peripheral clock
 * 				  for the given SPI port
 *
 * @param[in]	- base address of the GPIO peripheral
 * @param[in]	- ENABLE or DISABLE macros
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ***********************************************************************/
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	// check value of EnorDi parameter
	if(EnorDi == ENABLE)
	{
		// enable clock for given GPIO port address
		if (pSPIx == SPI1)
			SPI1_PCLK_EN();
		else if (pSPIx == SPI2)
			SPI2_PCLK_EN();
		else if (pSPIx == SPI3)
			SPI3_PCLK_EN();
		else if (pSPIx == SPI4)
			SPI4_PCLK_EN();
	}
	else
	{
		// disable clock for given SPI port address
		if (pSPIx == SPI1)
			SPI1_PCLK_DI();
		else if (pSPIx == SPI2)
			SPI2_PCLK_DI();
		else if (pSPIx == SPI3)
			SPI3_PCLK_DI();
		else if (pSPIx == SPI4)
			SPI4_PCLK_DI();
	}
}

/**************************************************************************
 * Initialize SPI port and pin
 * ************************************************************************
 * @fn			- SPI_Init
 *
 * @brief		- To configure various bit fields of the SPI registers.
 *
 * 				  The user application creates a variable of SPI_Handle_t type
 *				  and sends pointer of that variable to this function to initialize
 *				  the SPI port and pin.
 *
 * @param[in]	- pointer to the handle structure
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	// Peripheral clock enable
	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	// There are two control registers where you have to store configurable parameters.
	// The control register controls the peripheral.

	// Depending on the data communication, you have one or two data registers
	// in order to place user data.

	// And also, one or more status registers.
	// The status register is the house for various status flags (flag a event) during
	// operation of peripheral.

	// First, let's configure the SPI_CR1 register.
	// Store all config bit fields and then copy into SPI_CR1 register.
	uint32_t tempreg = 0; // initialize tempreg to 0

	// 1. Configure the device mode.
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << SPI_CR1_MSTR;
	// If SPI mode is master, 1 will be shifted to 2nd bit position.
	// If it is slave, 0 will be shifted to the 2nd bit position.

	// 2. Configure the bus configuration.
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		// bidi mode (15th bit) should be cleared
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
	} else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		// bidi mode should be set
		tempreg |= (1 << SPI_CR1_BIDIMODE);
	} else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		// bidi mode should be cleared, so we can achieve two line and unidirectional
		tempreg &= ~(1 << SPI_CR1_BIDIMODE);
		// RXONLY bit must be set.
		tempreg |= (1 << SPI_CR1_RXONLY);
	}

	// 3. Configure the clock speed.
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR;

	// 4. Configure the DFF.
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF;

	// 5. Configure the CPOL.
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL;

	// 6. Configure the CPHA.
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA;

	// All initialization is done and we can save the value of tempreg variable to CR1 register.c
	pSPIHandle->pSPIx->SPI_CR1 = tempreg;
	// Here we can use assignment operator b/c we freshly initialize the CR1 register.
}

/**************************************************************************
 * Deinitialize SPI port and pin
 * ************************************************************************
  * @fn			- SPI_DeInit
 *
 * @brief		- To deinitialize given SPI port and SPI pin.
 * 				- So we have to reset all registers of SPI port.
 * 				- (Use RCC reset registers to reset SPI peripherals.)
 *
 * @param[in]	- pointer to the SPI peripheral register structure
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	// reset for given SPI port address
	if (pSPIx == SPI1)
		SPI1_REG_RESET();
	else if (pSPIx == SPI2)
		SPI2_REG_RESET();
	else if (pSPIx == SPI3)
		SPI3_REG_RESET();
	else if (pSPIx == SPI4)
		SPI4_REG_RESET();
}


/**************************************************************************
 * Return flag reset (for transmitting data)
 * ************************************************************************
 * @fn			- SPI_SendData
 *
 * @brief		- send data to TX buffer
 * 				-
 *				-
 * @param[in]	- peripheral base address
 * @param[in]	- requested flag
 * @return		- true or false
 *
 * @Note		- none
 ****************************************************************************/
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	//while( ! (pSPIx->SPI_SR & (1 << SPI_SR_TXE) ) );

	return FLAG_RESET;
}

/**************************************************************************
 * Send or transmit data (blocking call or polling based code)
 * ************************************************************************
 * @fn			- SPI_SendData
 *
 * @brief		- Implementation of send data to TX buffer.
 * 				- Until all the bytes have been transferred, this "function will block".
 *				- There are two while loops.
 *				- Let's say 1000 bytes. Until all the 1000 bytes are transferred,
 *				- this function will not return. That is the meaning of "block".
 *
 * @param[in]	- pointer to the base address of SPI peripheral register structure
 * @param[in]	- pointer to the TX buffer
 * @param[in]	- size of data transfer (how many bytes this API should send)
 *
 * @return		- none
 *
 * @Note		- This is a blocking call.
 * 				- There is also interrupt flavor of this API, which is
 * 				- non-blocking and no while loops.s
 ****************************************************************************/
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	// loop until length is greater than 0
	while(Len > 0)
	{
		// 1. Wait until TXE is set
		// This while loop may hang permanently.
		// You need watchdog setup in order to restart your whole program.
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		// 2. If above return set, check the DFF bit in CR1
		if(pSPIx->SPI_CR1 & (1 << SPI_CR1_DFF))
		{
			// 16 bit DFF
			// 1. load the data into the DR
			// We have to typecast to uint16_t pointer type.
			// This pointer will now be 16-bit pointer type.
			// When we dereference this data, the value which comes out this expression
			// is 16-bit data.
			pSPIx->SPI_DR = *((uint16_t*)pTxBuffer);

			// You sent out 2 bytes of data. Length is decreased by 2.
			Len--;
			Len--;

			// Increment pTxBuffer to make it point to next data item.
			// You transferred 2 bytes into it, so that's why buffer has to be incremented by 2.
			// I am typecasting this pointer to uint16_t type and then incrementing
			// (which should be incremented by 2).
			(uint16_t*)pTxBuffer++;
		} else
		{
			// 8 bit DFF
			// Just dereferencing of this pointer, which is of uint8_t pointer type.
			pSPIx->SPI_DR = *pTxBuffer;
			// Length is decreased by 1.
			Len--;
			pTxBuffer++;
		}
	}
}

/**************************************************************************
 * Receive data
 * ************************************************************************
 * @fn			- SPI_ReceiveData
 *
 * @brief		-
 *
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	// loop until length is greater than 0
	while(Len > 0)
	{
		// 1. Wait until RXNE is set
		// This while loop may hang permanently.
		// You need watchdog setup in order to restart your whole program.
		while(SPI_GetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

		// 2. If above return set, check the DFF bit in CR1.
		if(pSPIx->SPI_CR1 & (1 << SPI_CR1_DFF))
		{
			// 16 bit DFF
			// 1. Load the data from DR to the Rxbuffer address

			// We have to typecast to uint16_t pointer type.
			// This pointer will now be 16-bit pointer type.
			// When we dereference this data, the value which comes out this expression
			// is 16-bit data.

			// This loads 2 bytes of data from DR into the Rxbuffer.
			*((uint16_t*)pRxBuffer) = pSPIx->SPI_DR;

			// You sent out 2 bytes of data. Length is decreased by 2.
			Len--;
			Len--;

			// Increment pTxBuffer to make it point to next data item.
			// You transferred 2 bytes into it, so that's why buffer has to be incremented by 2.
			// I am typecasting this pointer to uint16_t type and then incrementing
			// (which should be incremented by 2).

			// Increment the Rxbuffer to point to the next free memory address.
			(uint16_t*)pRxBuffer++;
		} else
		{
			// 8 bit DFF
			// Just dereferencing of this pointer, which is of uint8_t pointer type.
			*(pRxBuffer) = pSPIx->SPI_DR;
			// Length is decreased by 1.
			Len--;
			pRxBuffer++;
		}
	}
}

/**************************************************************************
 * Enable or disable the SPI peripheral
 * ************************************************************************
 * @fn			- SPI_PeripheralControl
 *
 * @brief		- Small function to enable or disable SPI peripheral.
 *
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	- enable or disable SPI peripheral
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	// enable peripheral
	if(EnOrDi == ENABLE)
	{
		pSPIx->SPI_CR1 |= (1 << SPI_CR1_SPE);
	} else // disable peripheral
	{
		pSPIx->SPI_CR1 &= ~(1 << SPI_CR1_SPE);
	}
}

/**************************************************************************
 * Enable or disable the SSI bit
 * ************************************************************************
 * @fn			- SPI_SSIConfig
 *
 * @brief		- Small function to enable or disable SSI bit.
 *
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	- enable or disable SPI peripheral
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	// enable peripheral
	if(EnOrDi == ENABLE)
	{
		pSPIx->SPI_CR1 |= (1 << SPI_CR1_SSI);
	} else // disable peripheral
	{
		pSPIx->SPI_CR1 &= ~(1 << SPI_CR1_SSI);
	}
}
/**************************************************************************
 * Enable or disable the SSOE bit
 * ************************************************************************
 * @fn			- SPI_SSOEConfig
 *
 * @brief		- Small function to enable or disable SSOE bit.
 *
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	- enable or disable SSOE bit
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi)
{
	// enable peripheral
	if(EnOrDi == ENABLE)
	{
		pSPIx->SPI_CR2 |= (1 << SPI_CR2_SSOE);
	} else // disable peripheral
	{
		pSPIx->SPI_CR2 &= ~(1 << SPI_CR2_SSOE);
	}
}
/**************************************************************************
 * Interrupt Configuration
 * ************************************************************************
 * @fn			- GPIO_IRQConfig
 *
 * @brief		- All of the configuration in this API is processor specific.
 *
 * @param[in]	-
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/


/**************************************************************************
 *
 * ************************************************************************
 * @fn			-
 *
 * @brief		-
 *
 * @param[in]	-
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/

/**************************************************************************
 * Interrupt Handling
 * ************************************************************************
 * @fn			- GPIO_IRQConfig
 *
 * @brief		- The ISRs for various IRQ numbers are already implemented.
 * 				- These ISRs are attributed as "weak ISRs". It is your responsibility
 * 				- to override the ISRs in your application.
 *
 * @param[in]	-
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
/*void GPIO_IRQHandling(uint8_t PinNumber)
{
	// clear the exti pr register corresponding to the pin number
	// if the PR bit position corresponding to this pin number is set, then
	// interrupt is pending
	if(EXTI->PR & (1 << PinNumber))
	{
		// clear pending register by writing '1'
		EXTI->PR |= (1 << PinNumber);
	}
}
*/
