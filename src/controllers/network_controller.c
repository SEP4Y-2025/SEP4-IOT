#include "controllers/network_controller.h"
#include "services/logger_service.h"
#include "wifi.h"
#include <string.h>
#include <stdio.h>

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

// bool network_controller_connect_ap(const char *ssid, const char *password) {
//     bool ok = (wifi_command_join_AP((char*)ssid, (char*)password) == WIFI_OK);
//     logger_service_log("AT+CWJAP \"%s\": %s", ssid, ok ? "OK" : "FAIL");
//     return ok;
// }

bool network_controller_disconnect_ap(void) {
    return (wifi_command_quit_AP() == WIFI_OK);
}

bool network_controller_is_ap_connected(void) {
    bool ok = (wifi_command_AT() == WIFI_OK);
    // We’ll get lots of these; log only on failure
    if (!ok) {
        logger_service_log("AT: no response (disconnected)");
    }
    return ok;
}


bool network_controller_tcp_open(const char *ip, uint16_t port) {
    bool ok = (wifi_command_create_TCP_connection(
                    (char*)ip, port, internal_tcp_cb,
                    (char*)net_rx_buffer
               ) == WIFI_OK);
    logger_service_log("AT+CIPSTART \"%s\",%u: %s", ip, (unsigned)port, ok ? "OK" : "FAIL");
    return ok;
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

WIFI_ERROR_MESSAGE_t network_controller_connect_ap(char *ssid, char *password, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer) {
    // connect to wifi
    WIFI_ERROR_MESSAGE_t wifi_res = wifi_command_join_AP(ssid, password);

    // Log result of WiFi connection
    char wifi_res_msg[128];
    sprintf(wifi_res_msg, "Error: %d \n", wifi_res);
    logger_service_log(wifi_res_msg);

    if (wifi_res != WIFI_OK)
    {
        logger_service_log("Error connecting to WiFi!\n");
        return wifi_res;
    }
    else
    {
        logger_service_log("Connected to WiFi!\n");
    }

    wifi_command_create_TCP_connection(broker_ip, broker_port, callback, callback_buffer);

    // Create and send MQTT connect packet
    unsigned char connect_buf[200];
    int connect_buflen = sizeof(connect_buf);
    int connect_len = mqtt_service_create_mqtt_connect_packet(connect_buf, connect_buflen);

    if (connect_len > 0)
    {
        logger_service_log("MQTT Connect packet created!\n");
    }

    WIFI_ERROR_MESSAGE_t mqtt_res = wifi_command_TCP_transmit(connect_buf, connect_len);

    if (mqtt_res != WIFI_OK)
    {
        logger_service_log("Error sending MQTT Connect packet!\n");
        return mqtt_res;
    }

    return WIFI_OK;
}
