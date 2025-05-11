#include "scheduler.h"
#include "periodic_task.h"
#include <stddef.h>

#define MAX_TASKS 5

typedef struct {
    uint32_t interval_ms;
    uint32_t last_run_ms;
    void (*task_fn)(void);
} ScheduledTask;

static volatile uint32_t _ms_count = 0;
static ScheduledTask scheduled_tasks[MAX_TASKS];
static uint8_t task_count = 0;

// Called every 1 ms
static void _tick_cb(void) {
    _ms_count++;
}

void scheduler_init(void) {
    periodic_task_init_a(_tick_cb, 1);
}

uint32_t scheduler_millis(void) {
    return _ms_count;
}

bool scheduler_elapsed(uint32_t *last_ms, uint32_t interval_ms) {
    uint32_t now = _ms_count;
    return ((now - *last_ms) >= interval_ms);
}

void scheduler_mark(uint32_t *last_ms) {
    *last_ms = _ms_count;
}

void scheduler_register(void (*fn)(void), uint32_t interval_ms) {
    if (task_count >= MAX_TASKS || fn == NULL) return;
    scheduled_tasks[task_count].interval_ms = interval_ms;
    scheduled_tasks[task_count].last_run_ms = _ms_count;
    scheduled_tasks[task_count].task_fn = fn;
    task_count++;
}

void scheduler_run_pending_tasks(void) {
    uint32_t now = _ms_count;
    for (uint8_t i = 0; i < task_count; i++) {
        if ((now - scheduled_tasks[i].last_run_ms) >= scheduled_tasks[i].interval_ms) {
            scheduled_tasks[i].last_run_ms = now;
            scheduled_tasks[i].task_fn();
        }
    }
}
