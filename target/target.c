#include "target.h"

#include "spi.h"
#include "timers.h"

static void gpio_init(void);
static void system_init(void);

static void gpio_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO1_M;
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M;

    PAD_CONFIG->PORT1_CFG |= (PAD_CFG_MODE1 << (PAD_CFG_MODE_LEN * LED2_PIN));

    GPIO1->DIRECTION_OUT |= (1 << LED2_PIN);

    GPIO1->OUTPUT |= (1 << LED2_PIN);
}

static void system_init(void)
{
    PM->CLK_APB_M_SET |= PM_CLOCK_EPIC_M | PM_CLOCK_PM_M;

    gpio_init();
    timers_init();
    spi_init();

    write_csr(mstatus, MSTATUS_MIE);
    write_csr(mie, MIE_MEIE);
}

void target_init(void)
{
    system_init();

    timers_lptim0_start();
}
