#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "pinmux.h"
#include "spi_control.h"
#include "spi_26VF064B.h"

#define NUM_SSI_DATA 4

/*
 * main.c
 */
int main(void)
{
	uint32_t pui32DataRx[NUM_SSI_DATA];
	
	// Set the clocking to run directly from the crystal
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
	    		SYSCTL_XTAL_16MHZ);
	
	// reset chip
	op_Reset(SSI0_BASE);

	// test SPI access by reading JEDEC ID
	op_JEDECID_Read(SSI0_BASE, pui32DataRx);

	// read status register
	op_RDSR(SSI0_BASE, pui32DataRx);

	// WREN
	op_WREN(SSI0_BASE);

	// read status register
	op_RDSR(SSI0_BASE, pui32DataRx);

	// WRDI
	op_WRDI(SSI0_BASE);

	// read status register
	op_RDSR(SSI0_BASE, pui32DataRx);

	// test chip erase
	op_Chip_Erase(SSI0_BASE);

	// global write protect disable
	op_Global_Blk_Unlock(SSI0_BASE);

	// test page program
	uint32_t writeData[4] = {0xAA, 0xBB, 0xCC, 0xDD};
	op_Page_Prog(SSI0_BASE, 0x2000, writeData, 4);

	// reset chip
	op_Reset(SSI0_BASE);

	// test read
	uint32_t readData[4] = {0xEE, 0xFF, 0x11, 0x22};
	op_Read_40M(SSI0_BASE, 0x2000, readData, 4);

	// test read starting 1 'later'
	// to confirm it isn't smoke and mirrors
	op_Read_40M(SSI0_BASE, 0x2001, readData, 4);

	// test read security ID by reading factory-programmed region
	uint32_t secID[8];
	op_Read_SecID(SSI0_BASE, 0x0000, secID, 8);


	return 0;
}
