// src/controllers/network_controller.c
#include "controllers/network_controller.h"
#include "services/logger_service.h"
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
    bool ok = (wifi_command_join_AP((char*)ssid, (char*)password) == WIFI_OK);
    logger_service_log("AT+CWJAP \"%s\": %s", ssid, ok ? "OK" : "FAIL");
    return ok;
}

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

uint8_t network_controller_scan_aps(wifi_ap_t *list,
                                    uint8_t max_aps,
                                    uint16_t timeout_s)
{
    if (wifi_scan_APs(timeout_s) != WIFI_OK) {
        logger_service_log("AP Scan failed");
        return 0;
    }

    const char *raw = wifi_get_scan_buffer();
    // make a modifiable copy
    char *buf = strdup(raw);
    if (!buf) return 0;

    uint8_t count = 0;
    char *saveptr = NULL;
    for (char *line = strtok_r(buf, "\r\n", &saveptr);
         line && count < max_aps;
         line = strtok_r(NULL, "\r\n", &saveptr))
    {
        if (strncmp(line, "+CWLAP:", 7) == 0) {
            wifi_ap_t ap = { .ssid = "", .rssi = -999 };
            // Example line: +CWLAP:(3,"MySSID",-60,"MAC",1)
            if (sscanf(line,
                       "+CWLAP:(%*d,\"%32[^\"]\",%hd",
                       ap.ssid, &ap.rssi) == 2)
            {
                logger_service_log("Found AP '%s' RSSI %d",
                                    ap.ssid, ap.rssi);
                list[count++] = ap;
            }
        }
    }
    free(buf);
    return count;
}



bool network_controller_connect_best(const wifi_credential_t *known,
                                     uint8_t                  known_len,
                                     uint16_t                 scan_timeout_s)
{
    // 1) Scan all APs
    #define MAX_SCAN_APS 20
    wifi_ap_t aps[MAX_SCAN_APS];
    uint8_t  found = network_controller_scan_aps(aps, MAX_SCAN_APS, scan_timeout_s);
    if (found == 0) {
        logger_service_log("connect_best: no APs found");
        return false;
    }

    // 2) For each known network, track its best RSSI
    int16_t best_rssi = -32768;
    int     best_idx  = -1;  // index into known[]

    for (uint8_t k = 0; k < known_len; k++) {
        int16_t max_rssi_for_this = -32768;
        for (uint8_t i = 0; i < found; i++) {
            if (strcmp(known[k].ssid, aps[i].ssid) == 0) {
                if (aps[i].rssi > max_rssi_for_this) {
                    max_rssi_for_this = aps[i].rssi;
                }
            }
        }
        if (max_rssi_for_this > best_rssi) {
            best_rssi = max_rssi_for_this;
            best_idx  = k;
        }
    }

    if (best_idx < 0) {
        logger_service_log("connect_best: none of the %u known SSIDs were in range", (unsigned)known_len);
        return false;
    }

    // 3) We have a winner
    const char *chosen_ssid = known[best_idx].ssid;
    const char *chosen_pwd  = known[best_idx].password;
    logger_service_log("connect_best: choosing '%s' (RSSI=%d)", chosen_ssid, best_rssi);

    // 4) Attempt the connection
    bool ok = network_controller_connect_ap(chosen_ssid, chosen_pwd);
    logger_service_log("connect_best: AT+CWJAP \"%s\" → %s",
                       chosen_ssid, ok ? "OK" : "FAIL");
    return ok;
}

