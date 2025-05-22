// mock_pump_driver.c
#include "mock_pump_driver.h"

int pump_state = 0;

void pump_driver_init(void) {
    pump_state = 0;
}

void pump_driver_on(void) {
    pump_state = 1;
}

void pump_driver_off(void) {
    pump_state = 0;
}

void pump_driver_activate(uint32_t duration_ms) {
    pump_state = 1;
    // Simulate delay without blocking
    pump_state = 0;
}
