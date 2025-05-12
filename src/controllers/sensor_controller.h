// controllers/sensor_controller.h
#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

/** Call once at startup to init the DHT22 and LDR hardware. */
void   sensor_controller_init(void);

/**  
 * Read *all* sensors and cache the latest values.  
 * Should be called whenever you want to sample (e.g. every 2 s).  
 */
void   sensor_controller_read(void);

/** Get the *last* successful reading. */
uint8_t  sensor_controller_get_temperature_decimal(void);
uint8_t  sensor_controller_get_temperature_integer(void);
uint8_t  sensor_controller_get_humidity_decimal(void);
uint8_t  sensor_controller_get_humidity_integer(void);
uint16_t sensor_controller_get_light(void);
uint16_t sensor_controller_get_soil(void);

#endif // SENSOR_CONTROLLER_H
