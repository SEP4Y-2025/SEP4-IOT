#ifndef RAIN_WATER_LEVEL_SENSOR_H
#define RAIN_WATER_LEVEL_SENSOR_H

#include <stdint.h>

// The type we return: a 10-bit ADC reading (0…1023)
typedef uint16_t rain_sensor_raw_t;

// The sensor API
void    rain_water_level_sensor_init(void);
rain_sensor_raw_t rain_water_level_sensor_read_raw(void);

#endif // RAIN_WATER_LEVEL_SENSOR_H
