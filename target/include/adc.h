#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "target.h"

#define ADC_TEST_PORT GPIO0
#define ADC_TEST_PIN GPIO_PIN_9

void adc_init(void);

#ifdef __cplusplus
}
#endif