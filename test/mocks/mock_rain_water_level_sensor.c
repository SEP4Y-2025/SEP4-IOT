// mock_rain_sensor.c
#include "mock_rain_sensor.h"

uint16_t mock_rain_raw_value = 512;

void rain_water_level_sensor_init(void) {
    // No-op
}

uint16_t rain_water_level_sensor_read_raw(void) {
    return mock_rain_raw_value;
}

float rain_water_level_sensor_read_cm(void) {
    return (mock_rain_raw_value / 1023.0f) * 10.0f;
}

uint8_t rain_water_level_sensor_read_percentage(void) {
    float percentage = (mock_rain_raw_value / 1023.0f) * 100.0f;
    float scaled = (percentage / 60.0f) * 100.0f;
    if (scaled > 100.0f) scaled = 100.0f;
    if (scaled < 0.0f) scaled = 0.0f;
    return (uint8_t)scaled;
}
