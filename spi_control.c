
#include "spi_control.h"

bool isSpiBaseSupp(uint32_t spiBase)
{
	switch(spiBase)
	{
	case SSI0_BASE:
		return true;
	default:
		// error; others not supported
		return false;
	}
}

int32_t spi_init(uint32_t spiBase, int32_t csMode)
{
	if(!isSpiBaseSupp(spiBase))
	{
		return errSpiBaseNotSupp;
	}

    //
    // Enable Peripheral Clocks
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    switch(csMode)
    {
    case manual_cs:
        // Configure pin PA3 for GPIO SSI0FSS
        // Allows manual chip select control
        GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
        GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_DIR_MODE_OUT);
        GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        break;
    case auto_cs:
        // Enable pin PA3 for SSI0 SSI0FSS
        // Alternate config, if manual chip select control is not needed
        GPIOPinConfigure(GPIO_PA3_SSI0FSS);
        GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_3);
    	break;
    default:
    	return errCSModeNotSupp;
    }

    // Enable pin PA5 for SSI0 SSI0TX
    GPIOPinConfigure(GPIO_PA5_SSI0TX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5);

    // Enable pin PA4 for SSI0 SSI0RX
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_4);

    // Enable pin PA2 for SSI0 SSI0CLK
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2);

    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);

    // Enable the SSI0 module.
    SSIEnable(SSI0_BASE);

	return 0;
}

int32_t spi_start(uint32_t spiBase, int32_t csMode)
{
	if(!isSpiBaseSupp(spiBase))
	{
		return errSpiBaseNotSupp;
	}

	uint32_t dummyReadData;
    // Read any residual data from the SSI port
    while(SSIDataGetNonBlocking(spiBase, &dummyReadData))
    {
    }

    if(csMode == auto_cs)
    {
    	// don't toggle CS manually in auto mode
    	return 0;
    }

    uint32_t port = 0;
    uint8_t pin = 0;
    switch(spiBase)
    {
    case SSI0_BASE:
    	port = GPIO_PORTA_BASE;
    	pin = GPIO_PIN_3;
    	break;
    default:
    	return errSpiBaseNotSupp;
    }

    // drive CS high, then low
    // todo: may need separate polarity control for other peripherals
    GPIOPinWrite(port, pin, pin);
    GPIOPinWrite(port, pin, 0);

	return 0;
}
int32_t spi_stop(uint32_t spiBase, int32_t csMode)
{
	if(!isSpiBaseSupp(spiBase))
	{
		return errSpiBaseNotSupp;
	}

	if(csMode == auto_cs)
	{
		// don't toggle CS manually in auto mode
		return 0;
	}

	uint32_t port = 0;
	uint8_t pin = 0;
	switch(spiBase)
	{
	case SSI0_BASE:
		port = GPIO_PORTA_BASE;
		pin = GPIO_PIN_3;
		break;
	default:
		return errSpiBaseNotSupp;
	}

	// Wait until SSI is done transferring all the data in the transmit FIFO.
	while(SSIBusy(spiBase))
	{
	}

	// drive CS high
	// todo: may need separate polarity control for other peripherals
	GPIOPinWrite(port, pin, pin);

	return 0;
}

int32_t spi_write(uint32_t spiBase, uint32_t* spiData, uint32_t spiCount)
{
	if(!isSpiBaseSupp(spiBase))
	{
		return errSpiBaseNotSupp;
	}

	uint32_t i = 0;

	for(i = 0; i < spiCount; i++)
	{
		SSIDataPut(spiBase, 0xFF & spiData[i]);
	}

	return 0;
}
int32_t spi_read(uint32_t spiBase, uint32_t* spiData, uint32_t spiCount)
{
	if(!isSpiBaseSupp(spiBase))
	{
		return errSpiBaseNotSupp;
	}

	uint32_t i = 0;

    for(i = 0; i < spiCount; i++)
    {
        //
        // Receive the data using the "blocking" Get function. This function
        // will wait until there is data in the receive FIFO before returning.
        //
        SSIDataGet(spiBase, &spiData[i]);

        //
        // Since we are using 8-bit data, mask off the MSB.
        //
        spiData[i] &= 0x00FF;
    }

	return 0;
}

int32_t spi_write_transact(uint32_t spiBase, uint32_t* spiData, uint32_t spiCount, int32_t csMode)
{
	// todo: actually handle errors
	spi_init(spiBase, csMode);
	spi_start(spiBase, csMode);
	spi_write(spiBase, spiData, spiCount);
	spi_stop(spiBase, csMode);

	return 0;
}
int32_t spi_writeread_transact(uint32_t spiBase, uint32_t* spiWriteData, uint32_t* spiReadData, uint32_t spiCount, int32_t csMode)
{
	// todo: actually handle errors
	spi_write_transact(spiBase, spiWriteData, spiCount, csMode);
	spi_read(spiBase, spiReadData, spiCount);

	return 0;
}
