#include "target.h"

#include "adc.h"
#include "spi.h"
#include "timers.h"
#include "uart.h"

#include "esch.h"
#include "esch_semaphore.h"

#include "os_tasks.h"

#include "mcp2515.h"

static int itoa(int num, unsigned char* str, int len, int base);

static const char WELCOME_STR[] = "=================\r\nMettem MIK32 Playground\r\n=================\r\n";

static uint8_t spi_test_data[] = {
    0x03, 0x0E, 0xFF,
    0x03, 0x0E, 0xFF,
    0x03, 0x0E
};

static volatile uint8_t dummy_counter = 0;
static esch_semaphore_t* can_irq_sem = NULL;
static esch_semaphore_t* can_tx_sem = NULL;
// static char uart_tx_buff[255] = { 0 };

int main(void)
{
    target_init();

    uart_tx(UART_1, (char*)WELCOME_STR, sizeof(WELCOME_STR));

    esch_init();

    // esch_task_create(SPI_TX_TASK_NAME, SPI_TX_TASK_INTERVAL_MS, SPI_TX_TASK_PRIORITY, spi_tx_task, NULL);
    esch_task_create(LED2_TASK_NAME, LED2_TASK_INTERVAL_MS, LED2_TASK_PRIORITY, led2_task, NULL);
    // esch_task_create(SPI_TASK_NAME, SPI_TASK_INTERVAL_MS, SPI_TASK_PRIORITY, spi_task, NULL);

    can_irq_sem = esch_semaphore_create();
    can_tx_sem = esch_semaphore_create();

    esch_semaphore_take(can_irq_sem);
    esch_semaphore_take(can_tx_sem);

    esch_run();

    for (;;) {
    }

    return 0;
}

void spi_task(void* arg)
{
    (void)arg;
    if (esch_semaphore_take(can_irq_sem) == ESCH_OK) {
        mcp2515_irq_handler();
    }

    if (esch_semaphore_take(can_tx_sem) == ESCH_OK) {
        spi_test_data[1] = SPI0->IntStatus;

        uint16_t timeout = 0;
        while (!ANALOG_REG->ADC_VALID) {
            if (timeout++ > ADC_TIMEOUT_DEFAULT) {
                break;
            }
        }

        *(uint16_t*)&spi_test_data[2] = ANALOG_REG->ADC_VALUE;
        mcp2515_send(0x01C4, MCP2515_ID_STD, MCP2515_RTR_DATA, spi_test_data, sizeof(spi_test_data));
    }
}

void trap_handler(void)
{
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
        spi_test_data[0] = (SPI0_INT_PORT->OUTPUT & SPI0_INT_PIN);
        dummy_counter++;

        esch_semaphore_give(can_irq_sem);
    }
}

void spi_tx_task(void* arg)
{
    (void)arg;

    if (spi_initialized()) {
        spi_test_data[7] = dummy_counter; // TEST ONLY
        esch_semaphore_give(can_tx_sem);
    }
}

void led2_task(void* arg)
{
    (void)arg;
    LED2_PORT->OUTPUT ^= (1 << LED2_PIN);
    uint16_t timeout = 0;
    while (!ANALOG_REG->ADC_VALID) {
        if (timeout++ > ADC_TIMEOUT_DEFAULT) {
            break;
        }
    }

    uint16_t v = ANALOG_REG->ADC_VALUE;
    char str[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int n = 5;
    itoa(v, (unsigned char*)str, n, 10);
    str[n] = '\r';
    // str[n + 1] = '\n';

    uart_tx(UART_1, str, n + 1);
}

static int itoa(int num, unsigned char* str, int len, int base)
{
    int sum = num;
    int i = 0;
    int j = 0;
    int digit;
    uint8_t a;

    do {
        digit = sum % base;
        if (digit < 0xA) {
            str[i++] = '0' + digit;
        } else {
            str[i++] = 'A' + digit - 0xA;
        }
        sum /= base;
    } while (sum && (i < (len - 1)));

    for (i = 0, j = len - 1; i < j; i++, j--) {
        a = str[i];
        str[i] = str[j];
        str[j] = a;
    }

    return 0;
}
