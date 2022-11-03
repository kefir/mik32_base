#include "target.h"
#include "timers.h"

static void led1_blink(void);
static void led2_blink(void);

static const uint32_t DELAY_TICKS = 1600000;
static const uint32_t LED_DELAY_MS = 125;

static volatile uint32_t led2_counter = 0;

int main(void)
{
    target_init();

    timers_lptim0_irq_callback_register(led2_blink);
    timers_lptim0_start();

    for (;;) {
        led1_blink();
    }

    return 0;
}

void trap_handler(void)
{
    if (EPIC->RAW_STATUS & (1 << EPIC_LPTIM0_INDEX)) {
        timers_lptim0_irq();
    }
}

static void led1_blink(void)
{
    GPIO_0->OUTPUT ^= (1 << PIN_LED2);
    for (volatile uint32_t i = 0; i < DELAY_TICKS; i++)
        ;
}

static void led2_blink(void)
{
    if (led2_counter == LED_DELAY_MS) {
        GPIO_1->OUTPUT ^= (1 << PIN_LED2);
        led2_counter = 0;
    }
    led2_counter++;
}
