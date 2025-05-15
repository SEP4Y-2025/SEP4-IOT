#ifndef PUMP_CONTROLLER_H
#define PUMP_CONTROLLER_H

#include <stdint.h>  // For uint32_t

// Initializes the pump controller (e.g., sets up the pump pin as output)
void pump_controller_init(void);

// Activates the pump for the specified duration (in seconds)
void activate_pump(uint32_t duration);

// Turns the pump on
void pump_on(void);

// Turns the pump off
void pump_off(void);

#endif // PUMP_CONTROLLER_H