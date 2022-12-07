#pragma once

#include "target.h"

typedef void (*IRQCallback)(void);

void timers_init(void);
void timers_lptim0_start(void);
void timers_lptim0_irq(void);
void timers_lptim0_irq_callback_register(IRQCallback callback);