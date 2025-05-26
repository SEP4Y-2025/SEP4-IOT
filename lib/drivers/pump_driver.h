#ifndef PUMP_DRIVER_H
#define PUMP_DRIVER_H

#include <stdint.h>  // For uint32_t

// Initializes the pump hardware (e.g., sets up the pump pin as output)
void pump_driver_init(void);

// Turns the pump on
void pump_driver_on(void);

// Turns the pump off
void pump_driver_off(void);

// Activates the pump for a specified duration (in milliseconds)
void pump_driver_activate(uint32_t duration_ms);

#endif // PUMP_DRIVER_H