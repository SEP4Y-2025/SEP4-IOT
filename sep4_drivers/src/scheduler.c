// scheduler.c
#include "scheduler.h"
#include "periodic_task.h"   // your Timer3-based 3-slot ISR driver

// Internal millisecond tick counter
static volatile uint32_t _ms_count = 0;

/**
 * @brief Called every 1 ms from the periodic_task ISR.
 */
static void _tick_cb(void) {
    _ms_count++;
}

void scheduler_init(void) {
    // Use the 'A' channel of periodic_task to call _tick_cb every 1 ms
    // (Note: periodic_task_init_a takes (func, interval_ms))
    periodic_task_init_a(_tick_cb, 1);
}

uint32_t scheduler_millis(void) {
    return _ms_count;
}

bool scheduler_elapsed(uint32_t *last_ms, uint32_t interval_ms) {
    uint32_t now = _ms_count;
    // handles wraparound correctly as long as intervals < 2^31
    if ((now - *last_ms) >= interval_ms) {
        return true;
    }
    return false;
}

void scheduler_mark(uint32_t *last_ms) {
    *last_ms = _ms_count;
}
