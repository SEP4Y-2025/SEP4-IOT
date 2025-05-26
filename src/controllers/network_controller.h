// src/controllers/network_controller.h
#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "wifi.h"

typedef void (*Network_TCP_Callback_t)(const uint8_t *data, uint16_t len);

bool network_controller_setup(void);

bool network_controller_is_ap_connected(void);

void network_controller_set_tcp_callback(Network_TCP_Callback_t cb);

bool network_controller_tcp_send(const uint8_t *data, uint16_t len);

WIFI_ERROR_MESSAGE_t network_controller_disconnect_ap(void);

bool network_controller_is_tcp_connected(void);

bool network_controller_tcp_close(void);

WIFI_ERROR_MESSAGE_t network_controller_tcp_open(const char *ip,
                                                 uint16_t port);

// Returns true and fills out_ssid/out_password on success,
// or false if no known AP was found or scan failed.
bool wifi_get_best_credentials(uint16_t timeout_s,
                               char *out_ssid,    // at least 33 bytes
                               char *out_password // at least 64 bytes
);

// stash the TCP callback & buffer once at startup:
void network_controller_init(void (*tcp_evt_cb)(void), char *tcp_evt_buf);

// join AP + open socket (no more callback args here)
WIFI_ERROR_MESSAGE_t network_controller_connect_ap(uint16_t timeout_s);

// just a status check
bool network_controller_is_ap_connected(void);

#endif // NETWORK_CONTROLLER_H
