#ifndef CLOCK_H
#define CLOCK_H

#define TIMER0 0x40
#define TIMER_MODE 0x43
#define RATE_GENERATOR 0x43
#define TIMER_FREQ 1193182L
#define HZ 100

extern void milli_delay(int milli_sec);

extern void clock_handler(int irq);

extern void init_clock();

int ticks;

#endif