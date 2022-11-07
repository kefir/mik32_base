#include "target.h"

#include "timers.h"

#include "mik32_spi.h"

static void gpio_init(void);
static void system_init(void);
static void spi0_init(void);

static void gpio_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_0_M;
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M;

    PAD_CONFIG->PORT_1_CFG |= (1 << (2 * LED1_PIN));
    PAD_CONFIG->PORT_0_CFG |= (1 << (2 * LED2_PIN));

    GPIO_1->DIRECTION_OUT = 0xFFFF;
    GPIO_0->DIRECTION_OUT = 0xFFFF;

    GPIO_1->OUTPUT |= (1 << LED1_PIN);
    GPIO_0->OUTPUT |= (1 << LED2_PIN);
}

static void system_init(void)
{
    PM->CLK_APB_M_SET |= PM_CLOCK_EPIC_M | PM_CLOCK_PM_M;

    gpio_init();
    timers_init();
    spi0_init();

    write_csr(mstatus, MSTATUS_MIE);
    write_csr(mie, MIE_MEIE);
}

static void spi0_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_SPI_0_M;

    SPI_0->Enable = ~SPI_ENABLE_M;

    SPI_0->Config = SPI_CONFIG_BAUD_RATE_DIV_32_M
        | SPI_CONFIG_CS_0_M
        | SPI_CONFIG_MASTER_M;

    SPI_0->TxThr = 1;

    SPI_0->Enable |= SPI_ENABLE_M;

    SPI_0->TxData = 0x03;
    while ((SPI_0->IntStatus & SPI_TX_FIFO_not_full_M)) {
    }

    SPI_0->TxData = 0x0C;
    while ((SPI_0->IntStatus & SPI_TX_FIFO_not_full_M)) {
    }

    volatile uint8_t lolol = SPI_0->RxData;
    (void)lolol;
}

void target_init(void)
{
    system_init();

    timers_lptim0_start();
}
