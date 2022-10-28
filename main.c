#include "csr.h"
#include "epic.h"
#include "gpio.h"
#include "mcu32_memory_map.h"
#include "pad_config.h"
#include "power_manager.h"
#include "scr1_csr_encoding.h"
#include "timer16.h"

#define __IRQ __attribute__((interrupt("machine")))
#define __IRQU __attribute__((interrupt("user")))

#define PIN_LED2 0 // LED2 управляется выводом PORT_1_0
#define DELAY_TICKS 1600000

static volatile uint32_t counter = 0;

void timer_init()
{
    PM->CLK_APB_P_SET |= PM_CLOCK_TIMER16_0_M;

    TIMER16_0->CR &= ~TIMER16_ENABLE_M;
    TIMER16_0->ARR = 999;
    TIMER16_0->IER = (1 << 1);
    TIMER16_0->CFGR = TIMER16_PRESCALER_32_M;

    EPIC->MASK_SET |= (1 << EPIC_TIMER16_0_INDEX);
}

void gpio_init()
{
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_0_M; // включение тактирования GPIO0 и GPIO1
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M; // включение тактирования блока для смены режима выводов

    PAD_CONFIG->PORT_1_CFG |= (1 << (2 * PIN_LED2)); // Установка порта 1 в режим GPIO
    PAD_CONFIG->PORT_0_CFG |= (1 << (2 * PIN_LED2)); // Установка порта 1 в режим GPIO

    GPIO_1->DIRECTION_OUT = 0xFFFF; // Установка направления порта 1 в выход
    GPIO_0->DIRECTION_OUT = 0xFFFF; // Установка направления порта 1 в выход

    GPIO_1->OUTPUT |= 1 << PIN_LED2;
    GPIO_0->OUTPUT |= 1 << PIN_LED2;
}

void led_blink()
{
    GPIO_0->OUTPUT |= 1 << PIN_LED2; //Установка значения вывода 3 порта 1 в высокий уровень
    for (volatile int i = 0; i < DELAY_TICKS; i++)
        ;
    GPIO_0->OUTPUT &= ~(1 << PIN_LED2); //Установка значения вывода 3 порта 1 в низкий уровень
    for (volatile int i = 0; i < DELAY_TICKS; i++)
        ;
}

void system_init()
{
    PM->CLK_APB_M_SET |= PM_CLOCK_EPIC_M | PM_CLOCK_PM_M;
}

int main()
{
    system_init();
    gpio_init();
    timer_init();

    write_csr(mstatus, MSTATUS_MIE);
    write_csr(mie, MIE_MEIE);

    bool first = true;

    while (1) {
        led_blink();
        if (first) {
            counter = 0;
            TIMER16_0->CR |= TIMER16_ENABLE_M | TIMER16_START_CONTIN_MODE_M;
            first = false;
        }
    }
}

void trap_handler_default(void)
{
    if (TIMER16_0->ISR & (1 << 1)) {
        TIMER16_0->ICR |= (1 << 1);

        if (++counter == 250) {
            GPIO_1->OUTPUT ^= 1 << PIN_LED2;
            counter = 0;
        }
    }
}