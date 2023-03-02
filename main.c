#include "target.h"

#include "adc.h"
#include "spi.h"
#include "timers.h"
#include "uart.h"

#include "esch.h"
#include "esch_semaphore.h"

#include "os_tasks.h"

#include "printf.h"

static const char WELCOME_STR[] = "=================\r\nMettem MIK32 Playground\r\n=================\r\n";

int main(void)
{
    target_init();

    uart_tx(UART_1, (char*)WELCOME_STR, sizeof(WELCOME_STR));

    esch_init();

    esch_task_create(LED2_TASK_NAME, LED2_TASK_INTERVAL_MS, LED2_TASK_PRIORITY, app_task, NULL);

    esch_run();

    for (;;) {
    }

    return 0;
}

void trap_handler(void)
{
    clear_csr(mstatus, MSTATUS_MIE);
    clear_csr(mie, MIE_MEIE);

    if (EPIC->RAW_STATUS & (1 << EPIC_LPTIM0_INDEX)) {
        timers_lptim0_irq();
    }

    if (EPIC->RAW_STATUS & (1 << EPIC_SPI0_INDEX)) {
        EPIC->MASK_LEVEL_CLEAR |= (1 << EPIC_SPI0_INDEX);
        if (SPI0->IntStatus & SPI_TX_FIFO_full_M) {
            SPI0->IntStatus |= SPI_TX_FIFO_full_M;
        }
        if (SPI0->IntStatus & SPI_TX_FIFO_not_full_M) {
            SPI0->IntStatus |= SPI_TX_FIFO_not_full_M;
        }
        if (SPI0->IntStatus & SPI_TX_FIFO_underflow_M) {
            SPI0->IntStatus |= SPI_TX_FIFO_underflow_M;
        }
    }

    if (EPIC->RAW_STATUS & (1 << EPIC_GPIO_IRQ_INDEX)) {
        EPIC->MASK_LEVEL_CLEAR |= (1 << EPIC_GPIO_IRQ_INDEX);
        GPIO_IRQ->CLEAR |= 1 << 7;
    }
    set_csr(mstatus, MSTATUS_MIE);
    set_csr(mie, MIE_MEIE);
}

void app_task(void* arg)
{
    (void)arg;
    LED2_PORT->OUTPUT ^= (1 << LED2_PIN);
    uint16_t timeout = 0;
    while (!ANALOG_REG->ADC_VALID) {
        if (timeout++ > ADC_TIMEOUT_DEFAULT) {
            break;
        }
    }

    uint16_t v = 4095 - ANALOG_REG->ADC_VALUE;
    char str[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    sprintf(str, "\rADC: %04d", v);

    uart_tx(UART_1, str, sizeof(str));
}
