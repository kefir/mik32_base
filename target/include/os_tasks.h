#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esch.h"

#define LED1_TASK_NAME "LED1_TASK"
static const uint32_t LED1_TASK_INTERVAL_MS = 250;
static const uint16_t LED1_TASK_PRIORITY = 0;
void led1_task(void* arg);

#define LED2_TASK_NAME "LED2_TASK"
static const uint32_t LED2_TASK_INTERVAL_MS = 500;
static const uint16_t LED2_TASK_PRIORITY = 1;
void led2_task(void* arg);

#ifdef __cplusplus
}
#endif