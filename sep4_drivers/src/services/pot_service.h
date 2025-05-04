#ifndef POT_SERVICE_H
#define POT_SERVICE_H

#include <stdint.h>

/** Called when “plant/pot/add” arrives. */
void pot_service_handle_add(const char *topic,
                            const uint8_t *payload,
                            uint16_t len);

/** Called when “plant/pot/remove” arrives. */
void pot_service_handle_remove(const char *topic,
                               const uint8_t *payload,
                               uint16_t len);

#endif // POT_SERVICE_H
