// services/sensor_service.h
#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

/**  
 * @param interval_ms  how often to take new samples (e.g. 2000 for 2 s)  
 */
void    sensor_service_init(uint32_t interval_ms);

/**  
 * Call every loop; once the interval has elapsed this will call  
 * sensor_controller_poll() and set new_data = true.  
 */
void    sensor_service_poll(void);

/**  
 * @return true if a fresh reading is available since the last get_*() call.  
 */
bool    sensor_service_has_new_data(void);

uint8_t   sensor_service_get_temperature_integer(void);
uint8_t   sensor_service_get_temperature_decimal(void);
uint8_t   sensor_service_get_humidity_integer(void);
uint8_t   sensor_service_get_humidity_decimal(void);
uint16_t sensor_service_get_light(void);
uint16_t sensor_service_get_soil(void);

#endif // SENSOR_SERVICE_H
