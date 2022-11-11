#include "target.h"

#include "spi.h"
#include "timers.h"

#include "esch.h"
#include "os_tasks.h"

static uint8_t spi_test_data[] = {
    0x31,
    0x69,
};

static void system_tick_timer(void);

int main(void)
{
    target_init();
    timers_lptim0_irq_callback_register(system_tick_timer);

    esch_init();

    esch_task_create(SPI_TX_TASK_NAME, SPI_TX_TASK_INTERVAL_MS, SPI_TX_TASK_PRIORITY, spi_tx_task, NULL);
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
    }
    if (EPIC->RAW_STATUS & (1 << EPIC_SPI0_INDEX)) {
        EPIC->MASK_LEVEL_CLEAR |= (1 << EPIC_SPI0_INDEX);
        if (SPI0->IntStatus & SPI_TX_FIFO_full_M) {
            SPI0->IntStatus &= ~SPI_TX_FIFO_full_M;
        }
        if (SPI0->IntStatus & SPI_TX_FIFO_not_full_M) {
            SPI0->IntStatus &= ~SPI_TX_FIFO_not_full_M;
        }
    }
}

void spi_tx_task(void* arg)
{
    (void)arg;

    SPI0_CS_ENABLE;
    spi_test_send_sync(SPI1, spi_test_data, sizeof(spi_test_data));
    SPI0_CS_DISABLE;
}

void led2_task(void* arg)
{
    (void)arg;
    LED2_PORT->OUTPUT ^= (1 << LED2_PIN);
}

static void system_tick_timer(void)
{
    esch_tick_advance();
}
