#include "clock.h"
#include "interrupt.h"
#include "package_iA32/packaging_iA32.h"
#include "process.h"

int ticks = 0;

void milli_delay(int milli_sec) {
    int t = get_ticks();
    while (((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}

void clock_handler(int irq) {
    ticks++;
    PROCESS* process = get_process_ready();
    process->ticks--;
    if (k_reenter != 0)
        return;

    if (process->ticks > 0)
        return;

    schedule();
}

void init_clock() {
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8)(TIMER_FREQ / HZ));
    out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8));
    put_irq_handler(CLOCK_IRQ, clock_handler);
    enable_irq(CLOCK_IRQ);
}
