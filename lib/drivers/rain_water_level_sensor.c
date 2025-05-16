#include "rain_water_level_sensor.h"
#include <Arduino.h>
#include <stdint.h>
#define RAIN_SENSOR_PIN A1 

void rain_water_level_sensor_init(void) {
    pinMode(RAIN_SENSOR_PIN, INPUT);
}

// Returns 0…1023
uint16_t rain_water_level_sensor_read_raw(void) {
    return (uint16_t)analogRead(RAIN_SENSOR_PIN);
}

// Convert raw to centimeters (0–10 cm in this example)
float rain_water_level_sensor_read_cm(void) {
    uint16_t raw = rain_water_level_sensor_read_raw();
    // map 0→0 cm, 1023→10 cm
    return (raw / 1023.0f) * 10.0f;
}

// uint8_t rain_water_level_sensor_read_percentage(void)
// {
//     uint16_t raw = rain_water_level_sensor_read_raw();
//     float percentage = (raw / 1023.0f) * 100.0f;

//     // Clamp to 0–100 range just in case
//     if (percentage > 100.0f) percentage = 100.0f;
//     if (percentage < 0.0f) percentage = 0.0f;

//     return (uint8_t)percentage;
// }

uint8_t rain_water_level_sensor_read_percentage(void)
{
    uint16_t raw = rain_water_level_sensor_read_raw();
    float percentage = (raw / 1023.0f) * 100.0f;

    // Adjust the range: 0–60% becomes 0–100%
    float scaled = (percentage / 60.0f) * 100.0f;

    // Clamp to 0–100 range
    if (scaled > 100.0f) scaled = 100.0f;
    if (scaled < 0.0f) scaled = 0.0f;

    return (uint8_t)scaled;
}

