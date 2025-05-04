#ifndef WINDOW_SERVICE_H
#define WINDOW_SERVICE_H

#include <stdint.h>

/** “plant/window” topic arrives with payload “OPEN” or “CLOSE”. */
void window_service_handle_toggle(const char *topic,
                                  const uint8_t *payload,
                                  uint16_t len);

#endif // WINDOW_SERVICE_H
