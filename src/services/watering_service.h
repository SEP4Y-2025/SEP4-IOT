
#ifndef WATERING_SERVICE_H
#define WATERING_SERVICE_H

#include <stdint.h>


void watering_service_init(void);
void watering_service_water_pot(void);
void watering_service_handle_settings_update(const char *topic, const uint8_t *payload, uint16_t len);

#endif 
