#ifndef MOCK_PUMP_DRIVER_H
#define MOCK_PUMP_DRIVER_H

#include <stdint.h>

void pump_driver_init(void);
void pump_driver_on(void);
void pump_driver_off(void);
void pump_driver_activate(uint32_t duration_ms);

extern int pump_state; // 0 = OFF, 1 = ON

#endif
