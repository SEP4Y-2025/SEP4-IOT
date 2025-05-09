#ifndef POT_SERVICE_H
#define POT_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

void pot_service_init(void);
bool pot_service_is_telemetry_enabled(void);
bool pot_service_is_watering_enabled(void);
void pot_service_handle_activate(const char *topic,
                            const uint8_t *payload,
                            uint16_t len);

void pot_service_handle_deactivate(const char *topic,
                               const uint8_t *payload,
                               uint16_t len);
bool pot_service_handle_get_pot_data(const char *topic,
                                 const uint8_t *payload,
                                 uint16_t len);

#endif // POT_SERVICE_H