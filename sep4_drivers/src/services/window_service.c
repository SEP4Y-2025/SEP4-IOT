#include "services/window_service.h"
#include <string.h>
#include "services/logger_service.h"

void window_service_handle_toggle(const char *topic,
                                  const uint8_t *payload,
                                  uint16_t len)
{
    logger_service_log("Window toggled");
}
