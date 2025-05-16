#include "pump_controller.h"
#include "../drivers/pump_driver.h"  // Adjusted path to locate the pump driver header file

void pump_controller_init(void) {
    pump_driver_init();  // Initialize the pump hardware using the driver
}

void activate_pump(uint32_t duration) {

    pump_driver_activate(duration);  // Activate the pump for the specified duration (convert seconds to ms)
}

void pump_on(void) {
    pump_driver_on();  // Use the driver to turn the pump on
}

void pump_off(void) {
    pump_driver_off();  // Use the driver to turn the pump off
}