#include "target.h"

#include "timers.h"

static void gpio_init(void);
static void system_init(void);

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

static void system_init(void)
{
    PM->CLK_APB_M_SET |= PM_CLOCK_EPIC_M | PM_CLOCK_PM_M;

    gpio_init();
    timers_init();

    write_csr(mstatus, MSTATUS_MIE);
    write_csr(mie, MIE_MEIE);
}

void target_init(void)
{
    system_init();
}
