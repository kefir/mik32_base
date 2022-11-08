#pragma once

#include "target.h"

#define SPI0_MOSI_PORT GPIO0
#define SPI0_MOSI_PIN GPIO_PIN_1

#define SPI0_MISO_PORT GPIO0
#define SPI0_MISO_PIN GPIO_PIN_0

#define SPI0_CLK_PORT GPIO0
#define SPI0_CLK_PIN GPIO_PIN_2

#define SPI0_CS_PORT GPIO0
#define SPI0_CS_PIN GPIO_PIN_4

void spi_init(void);
