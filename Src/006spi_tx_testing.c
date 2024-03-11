/*************************************************************************
 * We derived the pins. We will be using GPIO port B.
 * This code needs your driver APIs to transmit "Hello world" over MOSI line.
 * We don't want to use MISO and NSS, b/c there are no slaves.
 *
 *
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

	// MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_15;
	// Call GPIO_Init and pass address of handler.
	GPIO_Init(&SPIPins);

	// We are not using MISO and NSS. Only MOSI and clock.
	// These are the pin configurations in order to make the pins behave
	// as pins of the SPI2 peripheral.
	/************************************************************************
	// MISO
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	// Call GPIO_Init and pass address of handler.
	//GPIO_Init(&SPIPins);
	// NSS
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	// Call GPIO_Init and pass address of handler.
	//GPIO_Init(&SPIPins);
    *************************************************************************/

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
	// Peripheral clock will be divided by 2. This generates serial clock of 8Mhz.
	SPI2handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;
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
	// In this app, we don't have any slaves, so we don't need to use NSS.
	// So let's enable software slave management NSS pin
	SPI2handle.SPIConfig.SPI_SSM = SPI_SSM_EN;

	// Call SPI_Init and pass the address of handler.
	SPI_Init(&SPI2handle);
}

int main(void)
{
	// Let's create small user buffer.
	char user_data[] = "Hello world";

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

	// Pulling NSS to ground indicates that some other master has taken over bus.
	// The problem is keeping SSI to 0. It will solved by keeping SSI it 1.
	// NSS will be connected to Vdd internally and we can avoid the mode fault error.
	// The mode fault error happens in multi-master configuration.
	// We have to do before enabling the peripheral.
	SPI_SSIConfig(SPI2, ENABLE); // this makes NSS signal internally high and avoids MODF error

	// Before sending the data, the SPI peripheral needs to be enabled.
	SPI_PeriClockControl(SPI2, ENABLE);

	// user_data is of type char pointer, but you need uint8_t so typecast it
	SPI_SendData(SPI2, (uint8_t*)user_data, strlen(user_data));

	// Let's confirm SPI flag is not busy.
	// If this function keeps returning 1, that means SPI is busy.
	// If function returns 0, that means SPI is not busy. Loop will break and close the communication.
	while(SPI_GetFlagStatus(SPI2, SPI_BUSY_FLAG));

	// All data is sent to the external world, so we have to disable the peripheral.
	// After the last byte transmission, we close the SPI communication.
	// So MOSI and CLK are pulled to high. That's the idle state.
	SPI_PeriClockControl(SPI2, DISABLE);

	while(1); // infinite while loop to hang the application

	return 0;
}
