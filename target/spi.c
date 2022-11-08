#include "spi.h"

static void spi0_init(void);
static void gpio_init(void);

#define SPI0_CS_ENABLE (SPI0_CS_PORT->OUTPUT &= ~(1 << SPI0_CS_PIN))
#define SPI0_CS_DISABLE (SPI0_CS_PORT->OUTPUT |= (1 << SPI0_CS_PIN))

void spi_init(void)
{
    gpio_init();
    spi0_init();
}

static void gpio_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO0_M;

    PAD_CONFIG->PORT0_CFG
        |= PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * SPI0_CLK_PIN)
        | PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * SPI0_MOSI_PIN)
        | PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * SPI0_MISO_PIN)
        | PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * SPI0_CS_PIN);

    PAD_CONFIG->PORT0_PUPD
        |= PAD_PUPD_OFF << (PAD_PUPD_LEN * SPI0_CLK_PIN)
        | PAD_PUPD_OFF << (PAD_PUPD_LEN * SPI0_MISO_PIN)
        | PAD_PUPD_OFF << (PAD_PUPD_LEN * SPI0_MOSI_PIN)
        | PAD_PUPD_UP << (PAD_PUPD_LEN * SPI0_CS_PIN);

    // GPIO0->DIRECTION_OUT = (1 << SPI0_CS_PIN);
}

static void spi0_init(void)
{
    volatile uint32_t interrupt = 0x00;
    PM->CLK_APB_P_SET |= PM_CLOCK_SPI0_M;

    SPI0->Enable = ~SPI_ENABLE_M;

    SPI0->Config = SPI_CONFIG_BAUD_RATE_DIV_32_M
        | SPI_CONFIG_CS_0_M
        | SPI_CONFIG_MASTER_M;

    // SPI0->IntEnable |= SPI_TX_FIFO_not_full_M;

    SPI0->TxThr = 1;

    SPI0->Enable |= SPI_ENABLE_M;

    // SPI0_CS_ENABLE;

    SPI0->TxData = 0x03 & 0xFF;
    SPI0->TxData = 0x03 & 0xFF;

    // SPI0_CS_DISABLE;
}
