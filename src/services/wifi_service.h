
#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "wifi.h"

/**
 * Initialize the WiFi service layer.
 * - Calls your network_controller init + setup under the hood.
 * - Stores SSID/password for later.
 */
void wifi_service_init();

WIFI_ERROR_MESSAGE_t wifi_service_connect(const char *s, const char *p, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer);

/**
 * Drive the WiFi state machine.  
 * Call this every time through your main loop (e.g. 1–10 ms cadence).
 */
//void wifi_service_poll(void);

/**
 * Returns true if we’ve successfully joined the AP and hold an IP.
 */
bool wifi_service_is_connected(void);

#endif 
