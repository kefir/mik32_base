#include "mik32.h"

#include "csr.h"
#include "scr1_csr_encoding.h"

#include "epic.h"
#include "gpio.h"
#include "lptim.h"
#include "pad_config.h"
#include "power_manager.h"

#define __IRQ __attribute__((interrupt("machine")))
#define __IRQU __attribute__((interrupt("user")))

#define LPTIM_IT_RESET 0x4F

#define PIN_LED2 0 // LED2 управляется выводом PORT_1_0
#define DELAY_TICKS 1600000

static void timer_init(void);
static void gpio_init(void);
static void led_blink(void);
static void system_init(void);

static volatile uint32_t counter = 0;

static void timer_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_LPTIM0_M;

    LPTIM0->CR &= ~LPTIM_CR_ENABLE;
    LPTIM0->IER = LPTIM_IER_ARRM;
    LPTIM0->CFGR = LPTIM_CFGR_PSC32;
    LPTIM0->ARR = 999;
    LPTIM0->ICR = LPTIM_IT_RESET;
    LPTIM0->CMP = 1999;

    for (uint8_t i = 0; i < LPTIM_SYNC_TICKS; i++) {
        asm volatile("nop");
    }

    EPIC->MASK_SET |= (1 << EPIC_LPTIM0_INDEX);
}

static void gpio_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO_1_M | PM_CLOCK_GPIO_0_M; // включение тактирования GPIO0 и GPIO1
    PM->CLK_APB_M_SET |= PM_CLOCK_PAD_CONFIG_M; // включение тактирования блока для смены режима выводов

    PAD_CONFIG->PORT_1_CFG |= (1 << (2 * PIN_LED2)); // Установка порта 1 в режим GPIO
    PAD_CONFIG->PORT_0_CFG |= (1 << (2 * PIN_LED2)); // Установка порта 1 в режим GPIO

    GPIO_1->DIRECTION_OUT = 0xFFFF; // Установка направления порта 1 в выход
    GPIO_0->DIRECTION_OUT = 0xFFFF; // Установка направления порта 1 в выход

    GPIO_1->OUTPUT |= (1 << PIN_LED2);
    GPIO_0->OUTPUT |= (1 << PIN_LED2);
}

static void led_blink(void)
{
    GPIO_0->OUTPUT |= 1 << PIN_LED2; //Установка значения вывода 3 порта 1 в высокий уровень
    for (volatile int i = 0; i < DELAY_TICKS; i++)
        ;
    GPIO_0->OUTPUT &= ~(1 << PIN_LED2); //Установка значения вывода 3 порта 1 в низкий уровень
    for (volatile int i = 0; i < DELAY_TICKS; i++)
        ;
}

static void system_init(void)
{
    write_csr(mstatus, MSTATUS_MIE);
    write_csr(mie, MIE_MEIE);

    PM->CLK_APB_M_SET |= PM_CLOCK_EPIC_M | PM_CLOCK_PM_M;

    gpio_init();
    timer_init();
}

int main(void)
{
    system_init();

    bool first = true;

    while (1) {
        led_blink();
        if (first) {
            counter = 0;
            LPTIM0->CR |= LPTIM_CR_ENABLE | LPTIM_CR_CNTSTRT;
            first = false;
        }
    }
}

void trap_handler_default(void)
{
    if (LPTIM0->ISR & LPTIM_ISR_ARRM) {
        LPTIM0->ICR |= LPTIM_ICR_ARRM;

        if (++counter == 120) {
            GPIO_1->OUTPUT ^= 1 << PIN_LED2;
            counter = 0;
        }
    } else {
        LPTIM0->ICR |= LPTIM_IT_RESET;
    }
}