#pragma once

#include "wifi.h"
#include "stdint.h"

void initialize_system(void);
WIFI_ERROR_MESSAGE_t setup_network_connection(char *ssid, char *password, char *broker_ip, uint16_t broker_port, void (*callback)(void), unsigned char *callback_buffer);