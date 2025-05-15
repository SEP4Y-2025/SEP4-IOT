#include "rain_water_level_sensor.h"
#include <Arduino.h>

#define RAIN_SENSOR_PIN PA1   // AO pin

void rain_water_level_sensor_init(void) {
    pinMode(RAIN_SENSOR_PIN, INPUT);
}

// Returns 0…1023
rain_sensor_raw_t rain_water_level_sensor_read_raw(void) {
    return (rain_sensor_raw_t)analogRead(RAIN_SENSOR_PIN);
}

// Convert raw to centimeters (0–10 cm in this example)
float rain_water_level_sensor_read_cm(void) {
    uint16_t raw = rain_water_level_sensor_read_raw();
    // map 0→0 cm, 1023→10 cm
    return (raw / 1023.0f) * 10.0f;
}