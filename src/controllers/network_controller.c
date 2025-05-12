// src/controllers/network_controller.c
#include "controllers/network_controller.h"
#include "services/logger_service.h"
#include "services/mqtt_service.h"
// #include "services/wifi_service.h"
#include "wifi.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

/// Store user’s TCP callback and internal buffer
static Network_TCP_Callback_t user_cb = NULL;
#define NET_TCP_BUFFER_SIZE 512
static uint8_t net_rx_buffer[NET_TCP_BUFFER_SIZE];

static const wifi_credential_t known_credentials[] = {
    {.ssid = "JanPhone", .password = "Hello World"},
    // {.ssid = "Kamtjatka10", .password = "8755444387"},
    // …add more networks as needed…
};

static const char *broker_ip = "172.20.10.3";
static const uint16_t broker_port = 1883;

static const uint8_t known_credentials_len =
    sizeof(known_credentials) / sizeof(known_credentials[0]);

/// Internal “glue” for wifi to call when data arrives
static void internal_tcp_cb(void)
{
    // wifi driver will have filled net_rx_buffer and null-terminated it
    // find length:
    uint16_t len = strlen((char *)net_rx_buffer);
    if (user_cb)
    {
        user_cb(net_rx_buffer, len);
    }
}

bool wifi_get_best_credentials(uint16_t timeout_s,
                               char *out_ssid,
                               char *out_password)
{
    // 0) dump known list
    logger_service_log("get_best_creds: %u known SSIDs:", known_credentials_len);
    for (uint8_t k = 0; k < known_credentials_len; k++)
    {
        logger_service_log("  [%u] \"%s\"", k, known_credentials[k].ssid);
    }

    // 1) scan
    logger_service_log("get_best_creds: scanning (timeout=%us)...", timeout_s);
    if (wifi_scan_APs(timeout_s) != WIFI_OK)
    {
        logger_service_log("get_best_creds: scan failed");
        return false;
    }

    const char *raw = wifi_get_scan_buffer();
    uint16_t rawlen = wifi_get_scan_buffer_len();
    if (rawlen == 0)
    {
        logger_service_log("get_best_creds: no scan data");
        return false;
    }

    // 2) parse into small array
    typedef struct
    {
        char ssid[33];
        int rssi;
    } ap_t;
#define MAX_APS 20
    ap_t found[MAX_APS];
    uint8_t found_count = 0;

    char *copy = malloc(rawlen + 1);
    memcpy(copy, raw, rawlen);
    copy[rawlen] = '\0';

    char *saveptr, *line = strtok_r(copy, "\r\n", &saveptr);
    while (line && found_count < MAX_APS)
    {
        if (strncmp(line, "+CWLAP:", 7) == 0)
        {
            ap_t a = {.ssid = "", .rssi = -999};
            if (sscanf(line, "+CWLAP:(%*d,\"%32[^\"]\",%d", a.ssid, &a.rssi) == 2)
            {
                logger_service_log("  found [%u] '%s' (RSSI %d)",
                                   found_count, a.ssid, a.rssi);
                found[found_count++] = a;
            }
        }
        line = strtok_r(NULL, "\r\n", &saveptr);
    }
    free(copy);

    if (found_count == 0)
    {
        logger_service_log("get_best_creds: no APs parsed");
        return false;
    }

    // 3) match against known_credentials[]
    int best_rssi = INT_MIN;
    int best_idx = -1;
    for (uint8_t k = 0; k < known_credentials_len; k++)
    {
        for (uint8_t i = 0; i < found_count; i++)
        {
            if (strcmp(known_credentials[k].ssid, found[i].ssid) == 0)
            {
                if (found[i].rssi > best_rssi)
                {
                    best_rssi = found[i].rssi;
                    best_idx = k;
                }
            }
        }
    }

    if (best_idx < 0)
    {
        logger_service_log("get_best_creds: no known SSIDs found");
        return false;
    }

    // 4) copy out the winner
    strcpy(out_ssid, known_credentials[best_idx].ssid);
    strcpy(out_password, known_credentials[best_idx].password);
    logger_service_log("get_best_creds: chosen '%s' (%d dBm)",
                       out_ssid, best_rssi);

    return true;
}

void network_controller_init(void)
{
    // init UART & WiFi module
    wifi_init(); // :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}
}

bool network_controller_setup(void)
{
    // disable echo, set station mode, single-conn
    if (wifi_command_disable_echo() != WIFI_OK)
        return false;
    if (wifi_command_set_mode_to_1() != WIFI_OK)
        return false;
    if (wifi_command_set_to_single_Connection() != WIFI_OK)
        return false;
    return true;
}

// bool network_controller_connect_ap(const char *ssid, const char *password) {
//     bool ok = (wifi_command_join_AP((char*)ssid, (char*)password) == WIFI_OK);
//     logger_service_log("AT+CWJAP \"%s\": %s", ssid, ok ? "OK" : "FAIL");
//     return ok;
// }

bool network_controller_disconnect_ap(void)
{
    return (wifi_command_quit_AP() == WIFI_OK);
}

bool network_controller_is_ap_connected(void)
{
    bool ok = (wifi_command_AT() == WIFI_OK);
    // We’ll get lots of these; log only on failure
    if (!ok)
    {
        logger_service_log("AT: no response (disconnected)");
    }
    return ok;
}

bool network_controller_tcp_open(const char *ip, uint16_t port)
{
    bool ok = (wifi_command_create_TCP_connection(
                   (char *)ip, port, internal_tcp_cb,
                   (char *)net_rx_buffer) == WIFI_OK);
    logger_service_log("AT+CIPSTART \"%s\",%u: %s", ip, (unsigned)port, ok ? "OK" : "FAIL");
    return ok;
}

void network_controller_set_tcp_callback(Network_TCP_Callback_t cb)
{
    user_cb = cb;
}

bool network_controller_tcp_send(const uint8_t *data, uint16_t len)
{
    return (wifi_command_TCP_transmit((uint8_t *)data, len) == WIFI_OK);
}

bool network_controller_tcp_close(void)
{
    return (wifi_command_close_TCP_connection() == WIFI_OK);
}

WIFI_ERROR_MESSAGE_t network_controller_connect_ap(
    uint16_t timeout_s,
    void (*callback)(void), char *callback_buffer)
{
    char ssid[33], password[64];
    if (!wifi_get_best_credentials(timeout_s, ssid, password))
    {
        return WIFI_FAIL; // or some suitable error
    }

    // now exactly your existing join + MQTT logic:
    WIFI_ERROR_MESSAGE_t wifi_res = wifi_command_join_AP(ssid, password);
    char msg[128];
    sprintf(msg, "Error: %d\n", wifi_res);
    logger_service_log(msg);
    if (wifi_res != WIFI_OK)
    {
        logger_service_log("Error connecting to WiFi!\n");
        return wifi_res;
    }

    logger_service_log("Connected to WiFi!\n");
    wifi_command_create_TCP_connection(broker_ip, broker_port, callback, callback_buffer);

    unsigned char connect_buf[200];
    int len = mqtt_service_create_mqtt_connect_packet(connect_buf, sizeof(connect_buf));
    if (len > 0)
        logger_service_log("MQTT Connect packet created!\n");

    WIFI_ERROR_MESSAGE_t mqtt_res = wifi_command_TCP_transmit(connect_buf, len);
    if (mqtt_res != WIFI_OK)
    {
        logger_service_log("Error sending MQTT Connect packet!\n");
        return mqtt_res;
    }

    return WIFI_OK;
}