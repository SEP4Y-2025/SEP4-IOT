// mock_periodic_task.c
#include "mock_periodic_task.h"

static void (*mock_func_a)(void) = 0;
static void (*mock_func_b)(void) = 0;
static void (*mock_func_c)(void) = 0;

void periodic_task_init_a(void (*user_function_a)(void), uint32_t interval_ms_a) {
    mock_func_a = user_function_a;
}
void periodic_task_init_b(void (*user_function_b)(void), uint32_t interval_ms_b) {
    mock_func_b = user_function_b;
}
void periodic_task_init_c(void (*user_function_c)(void), uint32_t interval_ms_c) {
    mock_func_c = user_function_c;
}

void mock_trigger_task_a(void) {
    if (mock_func_a) mock_func_a();
}
void mock_trigger_task_b(void) {
    if (mock_func_b) mock_func_b();
}
void mock_trigger_task_c(void) {
    if (mock_func_c) mock_func_c();
}
