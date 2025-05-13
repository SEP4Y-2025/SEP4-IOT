#include "watering_service.h"
#include "config/device_config.h"
#include "services/logger_service.h"
#include "state/watering_state.h"
#include "controllers/sensor_controller.h"  // Include the sensor controller for reading soil moisture
#include <Arduino.h>  // Required for Arduino functions like digitalWrite and delay


#define MOISTURE_SENSOR_PIN A8  // Define the pin for soil moisture sensor
#define PUMP_PIN 30             // Define the pin for the pump (PC7)


void watering_service_init(void)
{

}
void watering_service_water_pot(void)
{
    if (is_watering_enabled()) {
        uint32_t dosage = get_water_dosage();  // Amount of water to use
        uint32_t tank_capacity = WATER_TANK_CAPACITY_ML;

        logger_service_log("Watering pot...\n");

        // **Check the water level before proceeding**
        uint8_t water_level = sensor_controller_get_water_level();  // Get the current water level

        if (water_level == 0) {
            logger_service_log("Water level is too low to water the plant.\n");
            return;  // Skip watering if the water level is too low
        }

        // **Read soil moisture value (assuming it returns a value from 0-1023)**
        int moisture_value = analogRead(MOISTURE_SENSOR_PIN);
        logger_service_log("Soil moisture level: %d", moisture_value);

        // **Determine watering time based on moisture level**
        if (moisture_value >= 300 && moisture_value < 500) {
            logger_service_log("Mildly dry: Pump ON for 1s");
            digitalWrite(PUMP_PIN, HIGH); // Turn the pump ON
            delay(1000);  // Pump runs for 1 second
            digitalWrite(PUMP_PIN, LOW);  // Turn the pump OFF
        } 
        else if (moisture_value >= 500 && moisture_value < 700) {
            logger_service_log("Moderately dry: Pump ON for 2s");
            digitalWrite(PUMP_PIN, HIGH); // Turn the pump ON
            delay(2000);  // Pump runs for 2 seconds
            digitalWrite(PUMP_PIN, LOW);  // Turn the pump OFF
        } 
        else if (moisture_value >= 700) {
            logger_service_log("Very dry: Pump ON for 4s");
            digitalWrite(PUMP_PIN, HIGH); // Turn the pump ON
            delay(4000);  // Pump runs for 4 seconds
            digitalWrite(PUMP_PIN, LOW);  // Turn the pump OFF
        } 
        else {
            logger_service_log("Soil is very wet, no need to water.");
        }

        // **After watering, optionally decrease the water level in the tank based on the dosage**
        decrease_water_level(dosage);  // Adjust water level after watering
    }
}

