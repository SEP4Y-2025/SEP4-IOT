#include "watering_service.h"
#include "config/device_config.h"
#include "services/logger_service.h"
#include "state/watering_state.h"
#include "controllers/sensor_controller.h" // Include the sensor controller for reading soil moisture
#include <pins_arduino.h>
#include <stdio.h>
#include "controllers/pump_controller.h" // Include the pump controller for pump operations
#include "state/watering_state.h"
#include "rain_water_level_sensor.h"

void watering_service_init(void)
{
    // Initialize necessary services, no changes required here
}

void watering_service_water_pot(void)
{
    // char buffer[64];
    // snprintf(buffer, sizeof(buffer), "Rain water level: %d", rain_water_level_sensor_read());
    // logger_service_log(buffer);
    if (true)
    {
    
        // uint32_t dosage = get_water_dosage();  // Amount of water to use
        uint32_t dosage = 25; // Amount of water to use
        logger_service_log("Watering pot...\n");

        // **Check the water level before proceeding**
        // uint8_t water_level = sensor_controller_get_water_level();  // Get the current water level
        // if (water_level == 0) {
        //     logger_service_log("Water level is too low to water the plant.\n");
        //     return;  // Skip watering if the water level is too low
        // }

        // **Read soil moisture value (assuming it returns a value from 0-1023)**
        // int moisture_value = analogRead(A8);
        // logger_service_log("Soil moisture level: %d", moisture_value);
        // pump_controller_init();  // Initialize the pump controller
        // **Dispense 25 mL of water every time it's required**
        // We calculate how long the pump should run to dispense 25 mL (in seconds or milliseconds)
        uint32_t watering_time_ms = 25.0 / 240000 * 3600 * 1000; // Pump runs for 0.375 seconds to dispense 25 mL
        logger_service_log("Watering time: %lu ms", watering_time_ms);
        activate_pump(watering_time_ms); // Activate the pump for the calculated time
    }
}
