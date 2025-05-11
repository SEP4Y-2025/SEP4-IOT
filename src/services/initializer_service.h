#pragma once

#include "wifi.h"
#include "stdint.h"

void initializer_service_initialize_system(void);
WIFI_ERROR_MESSAGE_t initializer_service_setup_network_connection(char *ssid, char *password, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer);