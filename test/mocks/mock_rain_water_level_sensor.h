#ifndef MOCK_RAIN_SENSOR_H
#define MOCK_RAIN_SENSOR_H

#include <stdint.h>

void rain_water_level_sensor_init(void);
uint16_t rain_water_level_sensor_read_raw(void);
float rain_water_level_sensor_read_cm(void);
uint8_t rain_water_level_sensor_read_percentage(void);

extern uint16_t mock_rain_raw_value;

#endif
