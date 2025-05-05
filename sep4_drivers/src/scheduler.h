// scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Install a 1 ms tick using periodic_task.
 *        Must be called once at startup, before any other scheduler calls.
 */
void scheduler_init(void);

/**
 * @brief Return the number of milliseconds since scheduler_init().
 */
uint32_t scheduler_millis(void);

/**
 * @brief Check whether 'interval_ms' has elapsed since 'last_ms'.
 * @param last_ms     Pointer to a timestamp variable (initialized to 0).
 * @param interval_ms How many milliseconds you want to wait.
 * @return true if (now - *last_ms) >= interval_ms.
 */
bool scheduler_elapsed(uint32_t *last_ms, uint32_t interval_ms);

/**
 * @brief Mark the current time into 'last_ms'.
 * @param last_ms Pointer to the timestamp you want to update.
 */
void scheduler_mark(uint32_t *last_ms);

#endif // SCHEDULER_H
