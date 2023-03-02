#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "target.h"

#define UART1_CLOCK PM_CLOCK_UART_1_M
#define UART1_BAUD 500000
// #define UART1_BAUD 115200

#define UART1_GPIO_PORT GPIO1
#define UART1_GPIO_CLK PM_CLOCK_GPIO1_M
#define UART1_GPIO_TX_PIN GPIO_PIN_9
#define UART1_GPIO_RX_PIN GPIO_PIN_8

void uart_init(void);
void uart_tx(UART_TypeDef* uart, char* buff, uint32_t len);

#ifdef __cplusplus
}
#endif
