#include "spi.h"

static void spi_clear_rx_buff(SPI_TypeDef* spi);
static void spi0_init(void);
static void gpio_init(void);

static bool init = false;

void spi_init(void)
{
    init = false;
    gpio_init();
    spi0_init();
    init = true;
}

static void gpio_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO0_M;

    PAD_CONFIG->PORT0_CFG
        |= PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * SPI0_CLK_PIN)
        | PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * SPI0_MOSI_PIN)
        | PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * SPI0_MISO_PIN)
        | PAD_CFG_MODE1 << (PAD_CFG_MODE_LEN * SPI0_CS_ALT_PIN)
        | PAD_CFG_MODE1 << (PAD_CFG_MODE_LEN * SPI0_CS_PIN);

    PAD_CONFIG->PORT0_PUPD
        |= PAD_PUPD_DOWN << (PAD_PUPD_LEN * SPI0_CLK_PIN)
        | PAD_PUPD_DOWN << (PAD_PUPD_LEN * SPI0_MISO_PIN)
        | PAD_PUPD_DOWN << (PAD_PUPD_LEN * SPI0_MOSI_PIN)
        | PAD_PUPD_UP << (PAD_PUPD_LEN * SPI0_CS_ALT_PIN)
        | PAD_PUPD_UP << (PAD_PUPD_LEN * SPI0_CS_PIN);

    GPIO0->DIRECTION_OUT
        |= (1 << SPI0_CS_PIN)
        | (1 << SPI0_CS_ALT_PIN)
        | (1 << SPI0_MOSI_PIN)
        | (1 << SPI0_CLK_PIN);

    GPIO0->DIRECTION_IN |= (1 << SPI0_MISO_PIN);
}

static void spi0_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_SPI0_M;

    SPI0->Enable = ~SPI_ENABLE_M;

    SPI0->Config = SPI_CONFIG_BAUD_RATE_DIV_64_M
        | SPI_CONFIG_Manual_CS_M
        | SPI_CONFIG_CS_NONE_M
        | SPI_CONFIG_PERI_SEL_M
        | SPI_CONFIG_MASTER_M;

    SPI0->IntEnable
        |= SPI_TX_FIFO_full_M
        | SPI_TX_FIFO_not_full_M
        | SPI_TX_FIFO_underflow_M;

    spi_clear_rx_buff(SPI0);

    SPI0->TxThr = 1;

    SPI0->Enable |= SPI_ENABLE_M;

    EPIC->MASK_LEVEL_SET |= (1 << EPIC_SPI0_INDEX);

    uint8_t reset = 0xC0;
    // spi0_cs_enable();
    spi_test_send_sync(SPI0, &reset, 1);
    // spi0_cs_disable();
    for (volatile uint32_t i = 0; i < 160000; i++) { }
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
        spi0_cs_enable();

        for (uint8_t i = 0; i < len; i++) {
            while (spi->IntStatus & SPI_TX_FIFO_full_M) {
            }
            SPI0->TxData = data[i];
        }

        // check empty flag
        volatile uint8_t timeout = 75;
        while (!(spi->IntStatus & SPI_TX_FIFO_not_full_M)) {
            if (--timeout == 0) {
                break;
            }
        }

        spi0_cs_disable();
    }
}

bool spi_initialized(void)
{
    return init;
}

inline void spi0_cs_enable(void)
{
    SPI0_CS_PORT->CLEAR |= (1 << SPI0_CS_PIN);
}

inline void spi0_cs_disable(void)
{
    SPI0_CS_PORT->SET |= (1 << SPI0_CS_PIN);
}