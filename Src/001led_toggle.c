#include "stm32f407xx.h"

void delay(void)
{
	for(uint32_t i = 0; i < 500000; i++);
}

int main(void)
{
	// create the variable for GPIO handle
	GPIO_Handle_t GpioLed;

	/*******************************************************************************
	 * Initialize this structure variable GpioLed
	 *******************************************************************************/
	GpioLed.pGPIOx = GPIOD; // select port
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12; // do pin configuration
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
	//GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;


	// Before configuring registers of the GPIO peripheral, enable the peripheral clock.
	GPIO_PeriClockControl(GPIOD, ENABLE); // enable the clock for port GPIOD

	// send the address of this structure variable and call the GPIO init API
	GPIO_Init(&GpioLed);

	// After enabling the clock and initialization, toggle.
	while(1)
	{
		GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
		delay(); // give small delay so we can notice toggle
	}

	return 0;
}
