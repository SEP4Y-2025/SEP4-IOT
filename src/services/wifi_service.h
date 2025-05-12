
#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "wifi.h"

void wifi_service_init();
WIFI_ERROR_MESSAGE_t wifi_service_connect(void);
bool wifi_service_is_connected(void);
void wifi_service_poll(void);

#endif
