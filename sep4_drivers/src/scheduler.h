#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

void scheduler_init(void);
uint32_t scheduler_millis(void);
bool scheduler_elapsed(uint32_t *last_ms, uint32_t interval_ms);
void scheduler_mark(uint32_t *last_ms);

// New multi-task support
void scheduler_register(void (*fn)(void), uint32_t interval_ms);
void scheduler_run_pending_tasks(void);

#endif
