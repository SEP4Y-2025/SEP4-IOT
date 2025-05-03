// src/controllers/network_controller.c
#include "controllers/wifi_controller.h"
#include "wifi.h"    // your existing wifi_command_* APIs :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}
#include <string.h>

/// Store user’s TCP callback and internal buffer
static Network_TCP_Callback_t user_cb = NULL;
#define NET_TCP_BUFFER_SIZE 512
static uint8_t net_rx_buffer[NET_TCP_BUFFER_SIZE];

/// Internal “glue” for wifi to call when data arrives
static void internal_tcp_cb(void) {
    // wifi driver will have filled net_rx_buffer and null-terminated it
    // find length:
    uint16_t len = strlen((char*)net_rx_buffer);
    if (user_cb) {
        user_cb(net_rx_buffer, len);
    }
}

void network_controller_init(void) {
    // init UART & WiFi module
    wifi_init();                              // :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}
}

bool network_controller_setup(void) {
    // disable echo, set station mode, single-conn
    if (wifi_command_disable_echo()   != WIFI_OK) return false;
    if (wifi_command_set_mode_to_1()  != WIFI_OK) return false;
    if (wifi_command_set_to_single_Connection() != WIFI_OK) return false;
    return true;
}

bool network_controller_connect_ap(const char *ssid, const char *password) {
    return (wifi_command_join_AP((char*)ssid, (char*)password) == WIFI_OK);  // :contentReference[oaicite:4]{index=4}:contentReference[oaicite:5]{index=5}
}

bool network_controller_disconnect_ap(void) {
    return (wifi_command_quit_AP() == WIFI_OK);
}

bool network_controller_is_ap_connected(void) {
    // a quick AT check; if module responds, assume link is OK
    return (wifi_command_AT() == WIFI_OK);
}

bool network_controller_tcp_open(const char *ip, uint16_t port) {
    // hands our internal buffer & internal callback to the wifi driver:
    return (wifi_command_create_TCP_connection(
                (char*)ip,
                port,
                internal_tcp_cb,
                (char*)net_rx_buffer
            ) == WIFI_OK);  // :contentReference[oaicite:6]{index=6}:contentReference[oaicite:7]{index=7}
}

void network_controller_set_tcp_callback(Network_TCP_Callback_t cb) {
    user_cb = cb;
}

bool network_controller_tcp_send(const uint8_t *data, uint16_t len) {
    return (wifi_command_TCP_transmit((uint8_t*)data, len) == WIFI_OK);
}

bool network_controller_tcp_close(void) {
    return (wifi_command_close_TCP_connection() == WIFI_OK);
}
