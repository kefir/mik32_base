#include "target.h"

#include "adc.h"
#include "spi.h"
#include "timers.h"

#include "esch.h"

static void gpio_init(void);
static void system_init(void);
static void system_tick_timer(void);

static volatile uint32_t systick = 0;

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
    adc_init();
    timers_init();
    timers_lptim0_irq_callback_register(system_tick_timer);
    timers_lptim0_start();

    write_csr(mstatus, MSTATUS_MIE);
    write_csr(mie, MIE_MEIE);

    spi_init();
}

void target_init(void)
{
    system_init();
}

void target_delay_ms(uint32_t delay)
{
    uint32_t tickstart = target_tick_get();
    uint32_t wait = delay;

    while ((target_tick_get() - tickstart) < wait) {
    }
}

uint32_t target_tick_get(void)
{
    return systick;
}

void target_systick_advance(void)
{
    systick++;
}

static void system_tick_timer(void)
{
    esch_tick_advance();
    target_systick_advance();
}
