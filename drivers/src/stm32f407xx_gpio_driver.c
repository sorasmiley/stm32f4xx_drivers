// In driver.c, you have to include respective peripheral's driver file.
#include "stm32f407xx_gpio_driver.h"
/**********************************************************************
 * Peripheral Clock setup
 * (Peripheral Control API)
 * ********************************************************************
 * @fn			- GPIO_PeriClockControl
 *
 * @brief		- This function enables or disables peripheral clock
 * 				  for the given GPIO port
 *
 * @param[in]	- base address of the GPIO peripheral
 * @param[in]	- ENABLE or DISABLE macros
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ***********************************************************************/
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	// check value of EnorDi parameter
	if(EnorDi == ENABLE)
	{
		// enable clock for given GPIO port address
		if (pGPIOx == GPIOA)
			GPIOA_PCLK_EN();
		else if (pGPIOx == GPIOB)
			GPIOB_PCLK_EN();
		else if (pGPIOx == GPIOC)
			GPIOC_PCLK_EN();
		else if (pGPIOx == GPIOD)
			GPIOD_PCLK_EN();
		else if (pGPIOx == GPIOE)
			GPIOE_PCLK_EN();
		else if (pGPIOx == GPIOF)
			GPIOF_PCLK_EN();
		else if (pGPIOx == GPIOG)
			GPIOG_PCLK_EN();
		else if (pGPIOx == GPIOH)
			GPIOH_PCLK_EN();
		else if (pGPIOx == GPIOI)
			GPIOI_PCLK_EN();
	}
	else
	{
		// disable clock for given GPIO port address
		if (pGPIOx == GPIOA)
			GPIOA_PCLK_DI();
		else if (pGPIOx == GPIOB)
			GPIOB_PCLK_DI();
		else if (pGPIOx == GPIOC)
			GPIOC_PCLK_DI();
		else if (pGPIOx == GPIOD)
			GPIOD_PCLK_DI();
		else if (pGPIOx == GPIOE)
			GPIOE_PCLK_DI();
		else if (pGPIOx == GPIOF)
			GPIOF_PCLK_DI();
		else if (pGPIOx == GPIOG)
			GPIOG_PCLK_DI();
		else if (pGPIOx == GPIOH)
			GPIOH_PCLK_DI();
		else if (pGPIOx == GPIOI)
			GPIOI_PCLK_DI();
	}
}

/**************************************************************************
 * Initialize GPIO port and pin
 * ************************************************************************
 * @fn			- GPIO_Init
 *
 * @brief		- To initialize given GPIO port and GPIO pin.
 *
 * 				  The user application creates a variable of GPIO_Handle_t type
 *				  and sends pointer of that variable to this function to initialize
 *				  the GPIO port and pin.
 *
 * @param[in]	- pointer to the handle structure
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp = 0; // temp register

	// In every peripheral initialization, enable the clock here itself.
	// So that user does not need to it explicitly.
	GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	/************************************************************************
	 * 1. Configure the mode of the GPIO pin (MODER)
	 ************************************************************************/
	// create macros for modes in gpio_driver.h

	// get mode by dereferencing
	// non-interrupt mode is 3(GPIO_MODE_ANALOG) or less
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		// Each pin takes 2 bit fields, so multiply by 2. Then shift (2 * pin number) bits to the left.
		temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		// Store temp into actual MODER register (at base address of peripheral)
		// must be bit-wise OR (do not touch other bits!!)
		pGPIOHandle->pGPIOx->MODER |= temp;
	} else { //interrupt mode
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			// 1. Configure the FTSR (register of EXTI)
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding RTSR bit
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			// 2. Configure the RTSR (register of EXTI)
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			// Clear the corresponding FTSR bit
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		} else if (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			// 3. Configure the RFTSR (register of EXTI)
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->FTSR |= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		//2. Configure GPIO port selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%4;

		// Send base address of GPIO peripheral and let macro return the GPIO port code.
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		//Before configuring the SYSCFG register, you have to enable the clock.
		SYSCFG_PCLK_EN();
		//Configure EXTI control register number and refer to section with port code.
		SYSCFG->EXTICR[temp1] = portcode << (temp2 * 4);

/*		//3. Enable the exti interrupt delivery (on EXTI line corresponding to pin number provided) using IMR
		EXTI->IMR |= 1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;*/
	}

	temp = 0;

	/************************************************************************
	 * 2. Configure the speed (OSPEEDR)
	 ************************************************************************/
	// 2 bits are dedicated to each pin, so multiply 2 and pin number
	// then shift the 2 bits over to correct pin (OSPEEDRX[1:0])
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	// Store temp into actual OSPEEDR register at base address of GPIO peripheral
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	temp = 0;

	/************************************************************************
	 * 3. Configure the pupd settings (PUPDR)
	 ************************************************************************/
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	temp = 0;

	/************************************************************************
	 * 4. Configure the output type (OTYPER)
	 ************************************************************************/
	temp = pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber;
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	temp = 0;

	/************************************************************************
	 * 5. Configure the alternate functionality register (ALTFN)
	 ************************************************************************/
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ALTFN)
	{
		// configure alt function registers
		uint8_t temp1, temp2;
		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

		//clearing
		pGPIOHandle->pGPIOx->MODER &= ~(0*3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		// setting
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4*temp2));
	}
}

/**************************************************************************
 * Deinitialize GPIO port and pin
 * ************************************************************************
 * @fn			- GPIO_DeInit
 *
 * @brief		- To deinitialize given GPIO port and GPIO pin.
 * 				- So we have to reset all registers of GPIO port.
 *
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{

	// reset for given GPIO port address
	if (pGPIOx == GPIOA)
		GPIOA_REG_RESET();
	else if (pGPIOx == GPIOB)
		GPIOB_REG_RESET();
	else if (pGPIOx == GPIOC)
		GPIOC_REG_RESET();
	else if (pGPIOx == GPIOD)
		GPIOD_REG_RESET();
	else if (pGPIOx == GPIOE)
		GPIOE_REG_RESET();
	else if (pGPIOx == GPIOF)
		GPIOF_REG_RESET();
	else if (pGPIOx == GPIOG)
		GPIOG_REG_RESET();
	else if (pGPIOx == GPIOH)
		GPIOH_REG_RESET();
	else if (pGPIOx == GPIOI)
		GPIOI_REG_RESET();
}

/**************************************************************************
 * Read from input pin
 * ************************************************************************
 * @fn			- GPIO_ReadFromInputPin
 *
 * @brief		- We have the pin number and we read the corresponding bit position in the IDR(input data register).
 * 				-
 *				-
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	- a small variable
 * @param[in]	-
 *
 * @return		- value at 0th bit position (0 or 1)
 *
 * @Note		- none
 ****************************************************************************/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;

	// We have to dereference the base address and select the IDR.
	// We right shift register by pin number and then mask values.
	// Then we typecast to uint8_t.
	value = (uint8_t)((pGPIOx->IDR>>PinNumber) & 0x00000001);

	return value;
}

/**************************************************************************
 * Read from input port
 * ************************************************************************
 * @fn			- GPIO_ReadFromInputPort
 *
 * @brief		- Same as read from input pin, but we return a slightly larger value.
 *				-
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	-
 * @param[in]	-
 *
 * @return		- entire port
 *
 * @Note		- none
 ****************************************************************************/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;

	// We have to dereference the base address and select the IDR.
	// We right shift register by pin number and then mask values.
	// Then we typecast to uint8_t.
	value = (uint16_t)pGPIOx->IDR;

	return value;
}

/**************************************************************************
 * Write to output port
 * ************************************************************************
 * @fn			- GPIO_WriteToOutputPin
 *
 * @brief		- We write a value 0 or 1 to the pin number.
 *				-
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	- pin number
 * @param[in]	- value is 0 or 1
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if (Value == GPIO_PIN_SET)
	{
		// write 1 to the output data register at the bit field corresponding to the pin number
		pGPIOx->ODR |= (1 << PinNumber);
	} else
	{
		// write 0 (clearing bit position corresponding to the pin number in the ODR register)
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/**************************************************************************
 * Write to output port
 * ************************************************************************
 * @fn			- GPIO_WriteToOutputPort
 *
 * @brief		- To write to whole port of given GPIO peripheral.
 *				-
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	- value is 0 or 1
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	// copy value into ODR register
	pGPIOx->ODR = Value;
}

/**************************************************************************
 * Toggle output pin
 * ************************************************************************
 * @fn			- GPIO_WriteToOutputPort
 *
 * @brief		- We use bitwise XOR operation to toggle the pin number.
 *				-
 * @param[in]	- pointer to the GPIO peripheral register structure
 * @param[in]	- pin number
 * @param[in]	-
 *
 * @return		- none
 *
 * @Note		- none
 ****************************************************************************/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	// ^ is XOR
	//pGPIOx->ODR = pGPIOx->ODR ^ (1 << PinNumber);
	pGPIOx->ODR ^= (1 << PinNumber);
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
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if(EnorDi == ENABLE)
	{
		// find out in what range the IRM number actually lies
		// There are 81 IRQ numbers implemented
		if(IRQNumber <= 31)
		{
			//program ISER0 register
			*NVIC_ICER0 |= (1 << IRQNumber); // dereference and put value
		} else if(IRQNumber > 31 && IRQNumber < 64) // 32 to 63
		{
			//program ISER1 register
			*NVIC_ICER1 |= (1 << IRQNumber % 32); // dereference and put value
		} else if(IRQNumber >= 64 && IRQNumber < 96) // 64 to 95
		{
			//program ISER2 register
			*NVIC_ICER2 |= (1 << IRQNumber % 64); // dereference and put value
		}
	}
}
/**************************************************************************
 * Priority Configuration
 * ************************************************************************
 * @fn			- GPIO_IRQPriorityConfig
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
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	// first lets find out the ipr register
	uint8_t iprx = IRQNumber/4;
	uint8_t iprx_section = IRQNumber%4;

	// The whole register is 32 bits, so that's why the next register will be 404.
	// So we multiply by 4.
	// iprx * 4
	//
	// IPR1 -> xx404		IRQ3_PRI | IRQ2_PRI | IRQ1_PRI | IRQ0_PRI
	// IPR0 -> xx400

	// IRQ0_PRI is 8 bits
	// lower 4 bits are not implemented for every priority field
	// top 4 bits are implemented

	// 8 multiplied by this section plus number of priority bits implemented
	uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

	// jumps to iprx's address
	// shift priority value by shift amount
	*(NVIC_PR_BASE_ADDR + (iprx *4)) |= (IRQPriority << shift_amount);
}

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
void GPIO_IRQHandling(uint8_t PinNumber)
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
