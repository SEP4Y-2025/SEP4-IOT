#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

void    sensor_service_init();
bool    sensor_service_read(void);

uint8_t   sensor_service_get_temperature_integer(void);
uint8_t   sensor_service_get_temperature_decimal(void);
uint8_t   sensor_service_get_humidity_integer(void);
uint8_t   sensor_service_get_humidity_decimal(void);
uint16_t sensor_service_get_light(void);
uint16_t sensor_service_get_soil(void);

#endif 
