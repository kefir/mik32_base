#include "spi.h"

#include "mcp2515.h"

#define SPI_TIMEOUT 32000

static uint8_t spi0_tx_rx(uint8_t byte);
static void spi_clear_rx_buff(SPI_TypeDef* spi);
static void can_test(void);
static void spi0_init(void);
static void gpio_init(void);

static bool init = false;

static mcp2515_t mcp_drv = {
    .disable = spi0_cs_disable,
    .enable = spi0_cs_enable,
    .transfer = spi0_tx_rx
};

void spi_init(void)
{
    init = false;
    gpio_init();
    spi0_cs_disable();
    spi0_init();

    can_test();

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

static uint8_t spi0_tx_rx(uint8_t byte)
{
    uint8_t ret = 0x00;
    uint32_t timeout = 0;

    while (SPI0->IntStatus & SPI_TX_FIFO_full_M) {
        if (++timeout == SPI_TIMEOUT) {
            break;
        }
    }
    SPI0->TxData = byte;

    timeout = 0;

    while (!(SPI0->IntStatus & SPI_RX_FIFO_not_empty_M)) {
        if (++timeout == SPI_TIMEOUT) {
            break;
        }
    }
    ret = SPI0->RxData;

    return ret;
}

static void can_test(void)
{
    mcp2515_init(&mcp_drv);
    if (mcp2515_speed(500000, 1, 1) < 0) {
        __asm__ volatile("nop");
    }
}

void spi_test_send_sync(SPI_TypeDef* spi, uint8_t* data_out, uint8_t* data, uint8_t len)
{
    if (spi && data) {
        for (uint8_t i = 0; i < len; i++) {
            data_out[i] = spi0_tx_rx(data[i]);
        }
    }
}

bool spi_initialized(void)
{
    return init;
}

void spi0_cs_enable(void)
{
    SPI0_CS_PORT->CLEAR |= (1 << SPI0_CS_PIN);
}

void spi0_cs_disable(void)
{
    SPI0_CS_PORT->SET |= (1 << SPI0_CS_PIN);
}