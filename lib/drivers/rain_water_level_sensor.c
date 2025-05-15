#include "rain_water_level_sensor.h"
#include <Arduino.h>  // for analogRead(), pinMode()

// Use the analog‐capable pin you wired your sensor to:
#define RAIN_SENSOR_PIN  PA1

// Initialize the analog pin
void rain_water_level_sensor_init(void) {
    pinMode(RAIN_SENSOR_PIN, INPUT);
}

// Read and return the 10-bit ADC level (0 = no water, 1023 = fully submerged)
rain_sensor_level_t rain_water_level_sensor_read(void) {
    return (rain_sensor_level_t)analogRead(RAIN_SENSOR_PIN);
}
