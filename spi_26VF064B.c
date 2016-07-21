
#include "spi_26VF064B.h"

#define csMode manual_cs

int32_t op_BusyWait(uint32_t spiBase)
{
	uint32_t statusReg[2];

	do {
		op_RDSR(spiBase, &statusReg[0]);
	} while(statusReg[1] & 0x01);

	return 0;
}

int32_t op_JEDECID_Read(uint32_t spiBase, uint32_t* spiData)
{
	uint32_t writeData[4];
	writeData[0] = cmd_JEDECID_Read;
	spi_writeread_transact(spiBase, writeData, spiData, 4, csMode);

    return 0;
}

int32_t op_RDSR(uint32_t spiBase, uint32_t* spiData)
{
	uint32_t writeData[2];
    writeData[0] = cmd_RDSR;
    spi_writeread_transact(spiBase, writeData, spiData, 2, csMode);

    return 0;
}

int32_t op_RDCR(uint32_t spiBase, uint32_t* spiData)
{
	uint32_t writeData[2];
    writeData[0] = cmd_RDCR;
    spi_writeread_transact(spiBase, writeData, spiData, 2, csMode);

    return 0;
}

int32_t op_Reset(uint32_t spiBase)
{
	uint32_t writeData[1];

	writeData[0] = cmd_RSTEN;
	spi_write_transact(spiBase, writeData, 1, csMode);

	writeData[0] = cmd_RST;
	spi_write_transact(spiBase, writeData, 1, csMode);

    return 0;
}

int32_t op_WREN(uint32_t spiBase)
{
	uint32_t writeData[1];

	writeData[0] = cmd_WREN;
	spi_write_transact(spiBase, writeData, 1, csMode);

    return 0;
}

int32_t op_WRDI(uint32_t spiBase)
{
	uint32_t writeData[1];

	writeData[0] = cmd_WRDI;
	spi_write_transact(spiBase, writeData, 1, csMode);

    return 0;
}

int32_t op_Chip_Erase(uint32_t spiBase)
{
	uint32_t writeData[1];

	op_WREN(spiBase);

	writeData[0] = cmd_Chip_Erase;
	spi_write_transact(spiBase, writeData, 1, csMode);

	op_BusyWait(spiBase);

	op_WRDI(spiBase);

	return 0;
}

int32_t op_Read_40M(uint32_t spiBase, uint32_t address, uint32_t* spiData, uint32_t byteCount)
{
	// from instruction 5.3 of flash programming manual
	// command is (4 bytes): command, MSB addr..LSB addr
	uint32_t commandData[4];
	commandData[0] = cmd_Read_40M;
	commandData[1] = (0xFF0000 & ((uint32_t)address)) >> 16;
	commandData[2] = (0xFF00 & ((uint32_t)address)) >> 8;
	commandData[3] = (0xFF & ((uint32_t)address));

	// todo: actually handle errors
	spi_init(spiBase, csMode);
	spi_start(spiBase, csMode);
	// write command...
	spi_write(spiBase, commandData, 4);
	// ...clear buffer...
	spi_read(spiBase, commandData, 4);
	// ...then dummy cycles...
	spi_write(spiBase, spiData, byteCount);
	// ...and finally read
	spi_read(spiBase, spiData, byteCount);
	spi_stop(spiBase, csMode);

	return 0;
}

int32_t op_Page_Prog(uint32_t spiBase, uint32_t address, uint32_t* spiData, uint32_t byteCount)
{
	// from instruction 5.20 of flash programming manual
	// command is (4 bytes): command, MSB addr..LSB addr
	uint32_t commandData[4];
	commandData[0] = cmd_Page_Prog;
	commandData[1] = (0xFF0000 & ((uint32_t)address)) >> 16;
	commandData[2] = (0xFF00 & ((uint32_t)address)) >> 8;
	commandData[3] = (0xFF & ((uint32_t)address));

	// write enable
	op_WREN(spiBase);

	// todo: actually handle errors
	spi_init(spiBase, csMode);
	spi_start(spiBase, csMode);
	// write command...
	spi_write(spiBase, commandData, 4);
	// ...then data to program
	spi_write(spiBase, spiData, byteCount);
	spi_stop(spiBase, csMode);

	op_BusyWait(spiBase);
	// write disable
	op_WRDI(spiBase);

	return 0;
}


