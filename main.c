#include "target.h"
#include "timers.h"

#include "esch.h"
#include "os_tasks.h"

static void system_tick_timer(void);

int main(void)
{
    target_init();
    timers_lptim0_irq_callback_register(system_tick_timer);

    esch_init();

    // esch_task_create(LED1_TASK_NAME, LED1_TASK_INTERVAL_MS, LED1_TASK_PRIORITY, led1_task, NULL);
    esch_task_create(LED2_TASK_NAME, LED2_TASK_INTERVAL_MS, LED2_TASK_PRIORITY, led2_task, NULL);

    esch_run();

    for (;;) {
    }

    return 0;
}

void trap_handler(void)
{
    if (EPIC->RAW_STATUS & (1 << EPIC_LPTIM0_INDEX)) {
        timers_lptim0_irq();
    } else if (EPIC->RAW_STATUS & (1 << EPIC_SPI0_INDEX)) {
        asm volatile("nop");
    }
}

void led1_task(void* arg)
{
    (void)arg;
}

void led2_task(void* arg)
{
    (void)arg;
    GPIO1->OUTPUT ^= (1 << LED2_PIN);
}

static void system_tick_timer(void)
{
    esch_tick_advance();
}
