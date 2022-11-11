#pragma once

#include <stddef.h>
#include <stdint.h>

#include "mik32.h"

#include "csr.h"
#include "scr1_csr_encoding.h"

#include "mik32_epic.h"
#include "mik32_gpio.h"
#include "mik32_lptim.h"
#include "mik32_pad_config.h"
#include "mik32_power_manager.h"
#include "mik32_spi.h"

#define LED2_PORT GPIO1
#define LED2_PIN GPIO_PIN_0

void target_init(void);