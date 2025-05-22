#ifndef MOCK_PERIODIC_TASK_H
#define MOCK_PERIODIC_TASK_H

#include <stdint.h>

void periodic_task_init_a(void (*user_function_a)(void), uint32_t interval_ms_a);
void periodic_task_init_b(void (*user_function_b)(void), uint32_t interval_ms_b);
void periodic_task_init_c(void (*user_function_c)(void), uint32_t interval_ms_c);

void mock_trigger_task_a(void);
void mock_trigger_task_b(void);
void mock_trigger_task_c(void);

#endif
