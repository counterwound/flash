#ifndef __SPI_CONTROL_H__
#define __SPI_CONTROL_H__

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

#define manual_cs	0
#define auto_cs		1

#define errSpiBaseNotSupp	-199
#define errCSModeNotSupp	-198

int32_t spi_init(uint32_t spiBase, int32_t csMode);
int32_t spi_start(uint32_t spiBase, int32_t csMode);
int32_t spi_stop(uint32_t spiBase, int32_t csMode);

int32_t spi_write(uint32_t spiBase, uint32_t* spiData, uint32_t spiCount);
int32_t spi_read(uint32_t spiBase, uint32_t* spiData, uint32_t spiCount);

int32_t spi_write_transact(uint32_t spiBase, uint32_t* spiData, uint32_t spiCount, int32_t csMode);
int32_t spi_writeread_transact(uint32_t spiBase, uint32_t* spiWriteData, uint32_t* spiReadData, uint32_t spiCount, int32_t csMode);

#endif //  __SPI_CONTROL_H__
