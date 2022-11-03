#include "timers.h"

static IRQCallback lptim0_callback = NULL;

static void lptim0_init(void)
{
    PM->CLK_APB_P_SET |= PM_CLOCK_LPTIM0_M;

    LPTIM0->CR &= ~LPTIM_CR_ENABLE;
    LPTIM0->IER = LPTIM_IER_ARRM;
    LPTIM0->CFGR = LPTIM_CFGR_PSC32 | LPTIM_CFGR_PRELOAD;
    LPTIM0->ARR = 999;

    EPIC->MASK_LEVEL_SET |= (1 << EPIC_LPTIM0_INDEX);

    LPTIM0->CR |= LPTIM_CR_ENABLE;

    for (volatile int i = 0; i < LPTIM_SYNC_TICKS; i++) {
    }
}

void timers_init(void)
{
    lptim0_init();
}

void timers_lptim0_start(void)
{
    LPTIM0->CR |= LPTIM_CR_CNTSTRT;
}

void timers_lptim0_irq(void)
{
    if (LPTIM0->ISR & LPTIM_ISR_ARRM) {
        LPTIM0->ICR |= LPTIM_ICR_ARRM;

        if (lptim0_callback) {
            lptim0_callback();
        }

        timers_lptim0_start();
    }
}

void timers_lptim0_irq_callback_register(IRQCallback callback)
{
    if (callback) {
        lptim0_callback = callback;
    }
}