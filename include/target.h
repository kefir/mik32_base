#pragma once

#include <stddef.h>
#include <stdint.h>

#include "mik32.h"

#include "csr.h"
#include "scr1_csr_encoding.h"

#include "epic.h"
#include "gpio.h"
#include "lptim.h"
#include "pad_config.h"
#include "power_manager.h"

#define PIN_LED2 0

void target_init(void);