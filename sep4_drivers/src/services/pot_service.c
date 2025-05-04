#include "services/pot_service.h"
#include "services/logger_service.h"
#include <stdlib.h>
#include <string.h>

void pot_service_handle_add(const char *topic,
                            const uint8_t *payload,
                            uint16_t len)
{
    // e.g. payload = ASCII ID; convert and call controller
    // char buf[16];
    // if (len < sizeof(buf)) {
    //   memcpy(buf, payload, len);
    //   buf[len] = '\0';
    //   int pot_id = atoi(buf);
    //   irrigation_controller_add_pot(pot_id);
    // }
    logger_service_log("Pot added");
}

void pot_service_handle_remove(const char *topic,
                               const uint8_t *payload,
                               uint16_t len)
{
    // char buf[16];
    // if (len < sizeof(buf)) {
    //   memcpy(buf, payload, len);
    //   buf[len] = '\0';
    //   int pot_id = atoi(buf);
    //   irrigation_controller_remove_pot(pot_id);
    // }
    logger_service_log("Pot removed");
}
