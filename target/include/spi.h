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

#define SPI0_INT_PORT GPIO0
#define SPI0_INT_PIN GPIO_PIN_7

void spi_init(void);
void spi_test_send_sync(SPI_TypeDef* spi, uint8_t* data_out, uint8_t* data, uint8_t len);
bool spi_initialized(void);

void spi0_cs_enable(void);
void spi0_cs_disable(void);
