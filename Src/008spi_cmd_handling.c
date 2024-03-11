/*************************************************************************
 * We derived the pins. We will be using GPIO port B.
 * This code needs your driver APIs to transmit "Hello world" over MOSI line.
 * We want to use MISO and NSS, b/c there is a Arduino slave.
 *
 * PB14 --> SPI2_MISO
 * PB15 --> SPI2_MOSI
 * PB13 --> SPI2_SCLK
 * PB12 --> SPI2_NSS
 * ALT function mode : 5s
 **************************************************************************/
#include <string.h>
// Do not forgot to include device specific header file.
#include "stm32f407xx.h"

#include <stdio.h>
extern void initialise_monitor_handles();

// Command codes that slave recognizes
#define COMMAND_LED_CTRL		0x50
#define COMMAND_SENSOR_READ		0x51
#define COMMAND_LED_READ		0x52
#define COMMAND_PRINT			0x53
#define COMMAND_ID_READ			0x54

#define LED_ON		1
#define LED_OFF		0

// Arduino analog pins
#define ANALOG_PIN0		0
#define ANALOG_PIN1		1
#define ANALOG_PIN2		2
#define ANALOG_PIN3		3
#define ANALOG_PIN4		4

#define LED_PIN			9


// time we are giving slave to be ready with data
void delay(void)
{
	for(uint32_t i = 0; i < 500000/2; i++); // 200 ms of gap
}

void SPI2_GPIOInits(void)
{
	GPIO_Handle_t SPIPins;

	// Let's initialize this handle variable.
	SPIPins.pGPIOx = GPIOB;
	// Initialize GPIO configuration structure.
	// We are using the pins for the SPI functionality.
	// That's why we have to choose mode as alternate function mode.
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	// Output type we can select push pull, b/c open drain is not required for SPI.
	// I2C needs to be open drain.
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	// This is not required.
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	// You can use whatever you want.
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	// I am configuring the serial clock(SCLK) first.
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	// Call GPIO_Init and pass address of handler.
	GPIO_Init(&SPIPins);

	// In this program, there will be all 4 lines of the SPI bus.
	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	// Call GPIO_Init and pass address of handler.
	GPIO_Init(&SPIPins);

	// We are using MISO, as well as MOSI, clock, and NSS.
	// These are the pin configurations in order to make the pins behave
	// as pins of the SPI2 peripheral.

	// MISO
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	// Call GPIO_Init and pass address of handler.
	GPIO_Init(&SPIPins);

	// NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	// Call GPIO_Init and pass address of handler.
	GPIO_Init(&SPIPins);


}

void SPI2_Inits(void)
{
	SPI_Handle_t SPI2handle;

	// initialize base address of SPI peripheral
	SPI2handle.pSPIx = SPI2;
	// bus configuration is full duplex
	SPI2handle.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	// Our app will behave as master. Our board will be acting as master.
	// We want to test our SPI_SendData API.
	SPI2handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	// Keep maxiumum possible.
	// Peripheral clock will be divided by 2. This generates serial clock of 2Mhz.
	// 16 / 8 = 2 Mhz
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8;
	// Let's use data frame format as 8 bits.
	SPI2handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;

	// We can keep CPOL as LOW. That is the default configuration.
	SPI2handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	// Let's change CPOL setting to HIGH.
	// The clock starts from HIGH to LOW.
	// After every data transmission, whenever there is an idle state
	// the clock pin will be in HIGH state.

	// CPHA is 0 or LOW.
	SPI2handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	// In this app, we have a Arduino slave, so we need to use NSS.
	// So let's disable the software slave management NSS pin and use hardware slave management instead.
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_DI;

	// Call SPI_Init and pass the address of handler.
	SPI_Init(&SPI2handle);
}

/***************************************************
 * Initialize the button
 ***************************************************/
void GPIO_ButtonInit(void)
{
	/*******************************************************************************
	 * Initialize this structure variable GpioBtn
	 *******************************************************************************/
	// create the variable for GPIO handle
	GPIO_Handle_t GpioBtn;

	GpioBtn.pGPIOx = GPIOA; // select port
	GpioBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0; // do pin configuration
	GpioBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN; // button is input mode
	GpioBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST; // fast speed

	// This is only applicable when mode is output.
	// GpioBtn.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;

	// We will be using an external pull down resister.
	GpioBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	// You don't need to do peripheral clock control here, b/c
	// we have included code in the driver itself.

	GPIO_Init(&GpioBtn);
}

void GPIO_LEDInit(void)
{
	/*******************************************************************************
	 * Initialize this structure variable GpioLed
	 *******************************************************************************/
	// create the variable for GPIO handle
	GPIO_Handle_t GpioLed;

	GpioLed.pGPIOx = GPIOD; // select port
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12; // do pin configuration
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT; // output mode
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW; // fast speed
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
	GPIO_PeriClockControl(GPIOD, ENABLE); // enable the clock for port GPIOD

	// send the address of this structure variable and call the GPIO init API
	GPIO_Init(&GpioLed);
}

uint8_t SPI_VerifyResponse(uint8_t ackbyte)
{
	if(ackbyte == 0xF5)
	{
		// ack
		return 1;
	} // else nack so return 0
	return 0;
}

int main(void)
{
	// 007spi_txonly_arduino.c
	// Let's create small user buffer.
	// char user_data[] = "Hello world";

	uint8_t dummy_write = 0xff;
	uint8_t dummy_read;

	initialise_monitor_handles();

	GPIO_ButtonInit();

	/*********************************************************************************
	 * Initialization
	 *********************************************************************************/
	// This function is used to initialize the GPIO pins to behave as SPI2 pins.
	SPI2_GPIOInits();
	// This is the peripheral configuration.
	SPI2_Inits();
	// After completing this, the peripheral is not active.
	// All the required configurations are loaded into the control registers of
	// SPI peripherals, but that does not mean peripheral is enabled.
	// You have to explicitly enable the SPI peripheral. For that we have a dedicated control bit 6 (SPE).
	// The control bit is 0 or disabled by default.

	// All configurations should be done when SPI peripheral is disabled.
	// When you enable SPI peripheral, it will be busy in data communication, so the SPI peripheral
	// will not accept any changes made.

	/************************************************************************
	 * SSI only makes sense when software slave management is enabled.
	 ************************************************************************/
	// Pulling NSS to ground indicates that some other master has taken over bus.
	// The problem is keeping SSI to 0. It will solved by keeping SSI it 1.
	// NSS will be connected to Vdd internally and we can avoid the mode fault error.
	// The mode fault error happens ins multi-master configuration.
	// We have to do before enabling the peripheral.
	// SPI_SSIConfig(SPI2, ENABLE); // this makes NSS signal internally high and avoids MODF error

	/************************************************************************
	 * SSOE bit
	 * making SSOE 1 does NSS output enable
	 * The NSS pin is automatically managed by the hardware.
	 * i.e when SPE=1, NSS will be pulled to low
	 * and NSS pin will be high when SPE=0
	 ************************************************************************/
	//Before enabling the peripheral, let's make SSOE as 1.
	SPI_SSOEConfig(SPI2, ENABLE);

	while(1)// infinite while loop to hang the application
	{
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0)); // The code should go back and hang here.
		// button is pressed if break out of while loop

		// To avoid button de-bouncing related issues, 200ms of delay.
		delay();

		/******************************************************************************************************
		 * 1. Enable the SPI peripheral and do the transmission only when button is pressed.
		 ******************************************************************************************************/
		SPI_PeriClockControl(SPI2, ENABLE); // Before sending the data, the SPI peripheral needs to be enabled.
		/******************************************************************************************************
		 * 2. Send first command
		 *
		 * CMD_LED_CTRL		<pin no(1)>		<value(1)>
		 ******************************************************************************************************/
		uint8_t commandcode = COMMAND_LED_CTRL; // 0x50
		uint8_t ackbyte;
		uint8_t args[2]; // arg[0] is pin number 9

		// Send command.
		SPI_SendData(SPI2, &commandcode, 1); // send only 1 byte
		// When slave receives this, it will check whether or not command is supported or not.
		// If slave supports this command, it will send ACK. So we have to now receive the ACK.
		// Remember that in SPI communications, SPI will not initiate the data transfer
		// so you have to send a dummy byte to the SPI and get that response back.
		// The slave is read with ACK or NACK byte in its shift register, but if you want to move that
		// data out of the slave shift register, you have to send some dummy byte.

		// After sending the data(command code), the master is going receive something in return.
		// For every transmission the master does, it is also going to receive one byte in return.
		// You have to do dummy read. RXNE has been set and you have to clear that.

		// do dummy read to clear off the RXNE
		// When you read the data register, RXNE will automatically be cleared.
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		// Send some dummy bits(1 byte of dummy data) to fetch the response from slave.
		SPI_SendData(SPI2, &dummy_write, 1);

		// Receive the data which arrived at the master.
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		// Compare whether you received ACK or NACK.
		// Call this function to see whether the ACK byte is valid or not.
		// If response is ACK(true), then only send arguments.

		// Fetch the ACK byte from slave using ackbyte variable.
		if ( SPI_VerifyResponse(ackbyte) )
		{
			// send other arguments of this command, pin number and value
			args[0] = LED_PIN; // we are going to connect LED to 9th pin of Arduino
			args[1] = LED_ON;
			// send data to the slave, you send args which is two bytes
			SPI_SendData(SPI2, args, 2);
		}
		// end of COMMAND_LED_CTRL

		/**************************************************
		 * CMD_SENSOR_READ	<analog pin number(1)>
		 *
		 * Connect analog pin to 5V and 3.3V.
		 * When reading 5V, it should be 255.
		 * When reading 3.3V, it should be 170.
		 **************************************************/

		// wait until button is pressed
		while( ! GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_0));
		// button is pressed if break out of while loop

		// To avoid button de-bouncing related issues, 200ms of delay.
		delay();

		commandcode = COMMAND_SENSOR_READ; // 0x51

		// Send command.
		SPI_SendData(SPI2, &commandcode, 1); // send only 1 byte

		// do dummy read to clear off the RXNE
		// When you read the data register, RXNE will automatically be cleared.
		SPI_ReceiveData(SPI2, &dummy_read, 1);

		// Send some dummy bits(1 byte of dummy data) to fetch the response from slave.
		SPI_SendData(SPI2, &dummy_write, 1);

		// Receive the data which arrived at the master.
		SPI_ReceiveData(SPI2, &ackbyte, 1);

		// Compare whether you received ACK or NACK.
		// Call this function to see whether the ACK byte is valid or not.
		// If response is ACK(true), then only send arguments.

		// ONLY IF WE RECEIVE ACK, we should implement IF code.
		// Fetch the ACK byte from slave using ackbyte variable.
		if ( SPI_VerifyResponse(ackbyte) )
		{
			// send other arguments of this command, pin number
			args[0] = ANALOG_PIN0; // we are going to connect LED to 9th pin of Arduino
			// send data to the slave, you send args which is one byte
			SPI_SendData(SPI2, args, 1); // send only 1 byte

			// Send always results in reception, so we are reading the data register to clear off the RXNE.
			// do a dummy read to clear off the RXNE b/c we have transmitted in IF block
			// When you read the data register, RXNE will automatically be cleared.
			SPI_ReceiveData(SPI2, &dummy_read, 1);

			// Slave is busy in ADC conversion, b/c that is analog read.
			// This will take some microseconds. But we are immediately asking slave for data and that is not good.
			// So insert some delay so that slave can be ready with the data.
			delay();


			// Transmit the dummy byte in order to receive response (fetch the analog sensor value)
			// Send some dummy bits(1 byte of dummy data) to fetch the response(acknowledgment) from slave.
			SPI_SendData(SPI2, &dummy_write, 1);

			uint8_t analog_read;
			SPI_ReceiveData(SPI2, &analog_read, 1);
		}
		// end of COMMAND_SENSOR_READ

		/*************************************************************************
		 * Disable peripheral SPI
		 * Whenever the SPI is busy in communication, the busy flag of SPI peripheral will be HIGH.
		 * The busy flag BSY can be found in the status register.
		 * If flag is 0, that means not busy. Its shift registers are actually at rest.
		 * If equal to 1, SPI is busy in communication.
		 *
		 * This flag is set and cleared by the hardware so you don't need to worry about it.
		 * But before closing the SPI communication, you need to first check this flag.
		 * If this flag is 0, then you close the communication.
		 *************************************************************************/
		// All data is sent to the external world, so we have to disable the peripheral.
		// After the last byte transmission, we close the SPI communication.
		// So MOSI and CLK are pulled to high. That's the idle state.

		// Let's confirm SPI flag is not busy.
		// If this function keeps returning 1, that means SPI is busy.
		// If function returns 0, that means SPI is not busy. Loop will break and close the communication.
		while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));
		SPI_PeriClockControl(SPI2, DISABLE);
	}

	return 0;
}
