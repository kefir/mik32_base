#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esch.h"

#define SPI_TX_TASK_NAME "SPI_TX_TASK"
static const uint32_t SPI_TX_TASK_INTERVAL_MS = 250;
static const uint16_t SPI_TX_TASK_PRIORITY = 0;
void spi_tx_task(void* arg);

#define LED2_TASK_NAME "LED2_TASK"
static const uint32_t LED2_TASK_INTERVAL_MS = 500;
static const uint16_t LED2_TASK_PRIORITY = 1;
void led2_task(void* arg);

#ifdef __cplusplus
}
#endif