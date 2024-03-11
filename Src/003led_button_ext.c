#include "stm32f407xx.h"

#define HIGH 			1
#define LOW				0
#define BTN_PRESSED 	LOW

void delay(void)
{
	for(uint32_t i = 0; i < 500000/2; i++);
}

int main(void)
{
	// create the variable for GPIO handle
	GPIO_Handle_t GpioLed, GpioBtn;

	/*******************************************************************************
	 * Initialize this structure variable GpioLed
	 *******************************************************************************/
	GpioLed.pGPIOx = GPIOA; // select port
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8; // do pin configuration
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT; // output mode
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST; // fast speed
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP; // Case#1: Push pull output type
	//GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD; // Case#2: Open drain output type

	// Case#1: Push pull output type
	// We don't have to enable any push pull resistor
	// because the pin output type is already push pull.
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	// Case#2: Open drain output type
	// We have to enable any push pull resistor b/c when you write 0
	// the LED can be pulled to GND, but it can't be pulled to VDD.
	// That is the reason the LED cannot toggle.
	// So to fix this activate the internal pull-up resistor.
	// A very tiny current will flow across LED.
	// To make the LED toggle at normal intensity
	// GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	// Before configuring registers of the GPIO peripheral, enable the peripheral clock.
	GPIO_PeriClockControl(GPIOA, ENABLE); // enable the clock for port GPIOD

	// send the address of this structure variable and call the GPIO init API
	GPIO_Init(&GpioLed);
/*
	// After enabling the clock and initialization, toggle.
	while(1)
	{
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		delay(); // give small delay so we can notice toggle
	}
*/

	/*******************************************************************************
	 * Initialize this structure variable GpioBtn
	 *******************************************************************************/
	GpioBtn.pGPIOx = GPIOB; // select port
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12; // do pin configuration
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN; // button is input mode
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST; // fast speed

	// This is only applicable when mode is output.
	// GpioBtn.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;

	// We will be using the internal pull up resister.
	//GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	// We will be using the external pull up resister.
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GpioBtn);

	while(1)
	{
		if(GPIO_ReadFromInputPin(GPIOB, GPIO_PIN_NO_12) == BTN_PRESSED)
		{
			delay(); // give small delay so we can notice toggle
			GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_8);
		}
	}

	return 0;
}
