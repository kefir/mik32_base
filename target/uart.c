#include "uart.h"

#include "mik32.h"
#include "mik32_pad_config.h"

#define UART1_DIVIDER (32000000 / UART1_BAUD)

static void gpio_init(void);

void uart_init(void)
{
    gpio_init();

    PM->CLK_APB_P_SET |= UART1_CLOCK;

    UART_1->CONTROL1 = 0x00;
    // UART_1->CONTROL3 |= UART_CONTROL3_OVRDIS_M;
    UART_1->DIVIDER = UART1_DIVIDER;

    UART_1->CONTROL1 |= UART_CONTROL1_TE_M | UART_CONTROL1_RE_M;

    UART_1->CONTROL1 |= UART_CONTROL1_UE_M;

    if (UART_1->CONTROL1 & UART_CONTROL1_TE_M) {
        while (!(UART_1->FLAGS & UART_FLAGS_TEACK_M)) { }
    }
    if (UART_1->CONTROL1 & UART_CONTROL1_RE_M) {
        while (!(UART_1->FLAGS & UART_FLAGS_REACK_M)) { }
    }
}

void uart_tx(UART_TypeDef* uart, char* buff, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        uart->TXDATA = (uint16_t)buff[i];
        while (!(uart->FLAGS & UART_FLAGS_TC_M)) { };
    }
}

static void gpio_init(void)
{
    PM->CLK_APB_P_SET |= UART1_GPIO_CLK;

    PAD_CONFIG->PORT1_CFG
        |= PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * UART1_GPIO_TX_PIN)
        | PAD_CFG_MODE0 << (PAD_CFG_MODE_LEN * UART1_GPIO_RX_PIN);

    PAD_CONFIG->PORT1_PUPD
        |= PAD_PUPD_OFF << (PAD_PUPD_LEN * UART1_GPIO_TX_PIN)
        | PAD_PUPD_UP << (PAD_PUPD_LEN * UART1_GPIO_RX_PIN);

    PAD_CONFIG->PORT1_DS
        |= PAD_DS_8MA << (PAD_DS_LEN * UART1_GPIO_TX_PIN)
        | PAD_DS_8MA << (PAD_DS_LEN * UART1_GPIO_RX_PIN);

    UART1_GPIO_PORT->DIRECTION_IN |= (1 << UART1_GPIO_RX_PIN);
    UART1_GPIO_PORT->DIRECTION_OUT |= (1 << UART1_GPIO_TX_PIN);
}
