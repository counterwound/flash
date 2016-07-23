#ifndef __SPI_26VF064B_H__
#define __SPI_26VF064B_H__

#include <stdint.h>
#include <stdbool.h>
#include "spi_control.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
#include "spi_control.h"

#define	cmd_JEDECID_Read	0x9F
#define cmd_RDSR			0x05
#define	cmd_RDCR			0x35
#define cmd_ULBPR			0x98
#define cmd_RSTEN			0x66
#define cmd_RST				0x99

#define cmd_Read_40M		0x03
#define cmd_Page_Prog		0x02

#define cmd_WREN			0x06
#define cmd_WRDI			0x04
#define cmd_Chip_Erase		0xC7
#define cmd_RSID			0x88
#define cmd_PSID			0xA5
#define cmd_LSID			0x85

// internal helpers
int32_t op_BusyWait(uint32_t spiBase);

// command implementations
int32_t op_JEDECID_Read(uint32_t spiBase, uint32_t* spiData);
int32_t op_RDSR(uint32_t spiBase, uint32_t* spiData);
int32_t op_RDCR(uint32_t spiBase, uint32_t* spiData);
int32_t op_Global_Blk_Unlock(uint32_t spiBase);
int32_t op_Reset(uint32_t spiBase);

int32_t op_WREN(uint32_t spiBase);
int32_t op_WRDI(uint32_t spiBase);
int32_t op_Chip_Erase(uint32_t spiBase);
int32_t op_Read_SecID(uint32_t spiBase, uint16_t address, uint32_t* spiData, uint32_t byteCount);

int32_t op_Read_40M(uint32_t spiBase, uint32_t address, uint32_t* spiData, uint32_t byteCount);
int32_t op_Page_Prog(uint32_t spiBase, uint32_t address, uint32_t* spiData, uint32_t byteCount);



#endif // __SPI_26VF064B_H__
