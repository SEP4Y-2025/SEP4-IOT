#ifndef RAIN_WATER_LEVEL_SENSOR_H
#define RAIN_WATER_LEVEL_SENSOR_H

#include <stdint.h>

void    rain_water_level_sensor_init(void);
uint16_t rain_water_level_sensor_read_raw(void);
uint8_t rain_water_level_sensor_read_percentage(void);


#endif // RAIN_WATER_LEVEL_SENSOR_H
