#include "adc.h"

static void gpio_init(void);

void adc_init(void)
{
    gpio_init();

    PM->CLK_APB_P_SET |= PM_CLOCK_ANALOG_REG_M;
    ANALOG_REG->ADC_CONFIG
        |= ADC_CONFIG_EN
        | (ADC_CHANNEL5 << ADC_SEL_S)
        | ADC_CONFIG_RESET_N;
    ANALOG_REG->ADC_CONTINIUS = 1;
}

static void gpio_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_GPIO0_M;

    PAD_CONFIG->PORT0_CFG
        |= PAD_CFG_MODE3 << (PAD_CFG_MODE_LEN * ADC_TEST_PIN);

    PAD_CONFIG->PORT0_PUPD
        |= PAD_PUPD_OFF << (PAD_PUPD_LEN * ADC_TEST_PIN);

    GPIO0->DIRECTION_IN
        |= (1 << ADC_TEST_PIN);
}