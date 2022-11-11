#include "spi.h"

static void spi_clear_rx_buff(SPI_TypeDef* spi);
static void spi0_init(void);
static void gpio_init(void);

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
        |= PAD_PUPD_DOWN << (PAD_PUPD_LEN * SPI0_CLK_PIN)
        | PAD_PUPD_DOWN << (PAD_PUPD_LEN * SPI0_MISO_PIN)
        | PAD_PUPD_DOWN << (PAD_PUPD_LEN * SPI0_MOSI_PIN)
        | PAD_PUPD_UP << (PAD_PUPD_LEN * SPI0_CS_PIN);

    GPIO0->DIRECTION_OUT
        |= (1 << SPI0_MOSI_PIN)
        | (1 << SPI0_CS_PIN)
        | (1 << SPI0_CLK_PIN);

    GPIO0->DIRECTION_IN |= (1 << SPI0_MISO_PIN);
}

static void spi0_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_SPI0_M;

    SPI0->Enable = ~SPI_ENABLE_M;

    SPI0->Config = SPI_CONFIG_BAUD_RATE_DIV_64_M
        | SPI_CONFIG_CS_0_M
        | SPI_CONFIG_MASTER_M;

    SPI0->IntEnable |= SPI_TX_FIFO_full_M;

    spi_clear_rx_buff(SPI0);

    SPI0->TxThr = 1;

    SPI0->Enable |= SPI_ENABLE_M;

    // EPIC->MASK_LEVEL_SET |= (1 << EPIC_SPI0_INDEX);
}

static void spi_clear_rx_buff(SPI_TypeDef* spi)
{
    if (spi) {
        while (spi->IntStatus & SPI_RX_FIFO_not_empty_M) {
            (void)spi->RxData;
        }
    }
}

void spi_test_send_sync(SPI_TypeDef* spi, uint8_t* data, uint8_t len)
{
    if (spi && data) {
        for (uint8_t i = 0; i < len; i++) {
            while (spi->IntStatus & SPI_TX_FIFO_full_M) {
            }
            SPI0->TxData = data[i];
        }
    }
}
