// src/controllers/network_controller.c
#include "controllers/network_controller.h"
#include "services/logger_service.h"
#include "wifi.h" // your existing wifi_command_* APIs :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}
#include <string.h>
#include <limits.h>

/// Store user’s TCP callback and internal buffer
static Network_TCP_Callback_t user_cb = NULL;
#define NET_TCP_BUFFER_SIZE 512
static uint8_t net_rx_buffer[NET_TCP_BUFFER_SIZE];

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

bool network_controller_connect_ap(const char *ssid, const char *password)
{
    bool ok = (wifi_command_join_AP((char *)ssid, (char *)password) == WIFI_OK);
    logger_service_log("AT+CWJAP \"%s\": %s", ssid, ok ? "OK" : "FAIL");
    return ok;
}

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

bool network_controller_connect_best(const wifi_credential_t *known,
                                     uint8_t known_len,
                                     uint16_t timeout_s)
{
    // 0) Dump your known‐networks list
    logger_service_log("connect_best: %u known SSIDs:", known_len);
    for (uint8_t k = 0; k < known_len; k++)
    {
        logger_service_log("    [%u] \"%s\"", k, known[k].ssid);
    }

    logger_service_log("connect_best: scanning (timeout=%us)...", timeout_s);
    if (wifi_scan_APs(timeout_s) != WIFI_OK)
    {
        logger_service_log("connect_best: scan failed");
        return false;
    }

    const char *raw = wifi_get_scan_buffer();
    uint16_t rawlen = wifi_get_scan_buffer_len();
    logger_service_log("connect_best: raw scan length = %u bytes", rawlen);
    if (rawlen == 0)
    {
        logger_service_log("connect_best: buffer empty, abort");
        return false;
    }

    // 1) Parse & log every found AP into a small array
    typedef struct
    {
        char ssid[33];
        int rssi;
    } ap_t;
#define MAX_APS 20
    ap_t found[MAX_APS];
    uint8_t found_count = 0;

    logger_service_log("connect_best: parsing found SSIDs:");
    char *copy = malloc(rawlen + 1);
    memcpy(copy, raw, rawlen);
    copy[rawlen] = '\0';

    char *saveptr, *line = strtok_r(copy, "\r\n", &saveptr);
    while (line && found_count < MAX_APS)
    {
        if (strncmp(line, "+CWLAP:", 7) == 0)
        {
            ap_t a = {.ssid = "", .rssi = -999};
            if (sscanf(line,
                       "+CWLAP:(%*d,\"%32[^\"]\",%d",
                       a.ssid, &a.rssi) == 2)
            {
                logger_service_log("  [%u] '%s' (RSSI %d)",
                                   found_count, a.ssid, a.rssi);
                found[found_count++] = a;
            }
        }
        line = strtok_r(NULL, "\r\n", &saveptr);
    }
    free(copy);

    if (found_count == 0)
    {
        logger_service_log("connect_best: no AP entries parsed");
        return false;
    }

    // 2) Now match against known[]
    int16_t best_rssi = INT_MIN;
    int best_idx = -1;
    for (uint8_t k = 0; k < known_len; k++)
    {
        for (uint8_t i = 0; i < found_count; i++)
        {
            if (strcmp(known[k].ssid, found[i].ssid) == 0)
            {
                logger_service_log("connect_best: '%s' matches found[%u] (RSSI %d)",
                                   known[k].ssid, i, found[i].rssi);
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
        logger_service_log("connect_best: none of the known SSIDs were found");
        return false;
    }

    // 3) Finally, join the best one
    const char *chosen = known[best_idx].ssid;
    logger_service_log("connect_best: best = '%s' (%d dBm), joining…",
                       chosen, best_rssi);
    WIFI_ERROR_MESSAGE_t res = wifi_command_join_AP(
        (char *)chosen,
        (char *)known[best_idx].password);
    if (res == WIFI_OK)
    {
        logger_service_log("connect_best: joined '%s' OK", chosen);
        return true;
    }
    else
    {
        logger_service_log("connect_best: join '%s' failed (err=%d)",
                           chosen, res);
        return false;
    }
}

int network_controller_tcp_receive(uint8_t *buf, uint16_t maxlen)
{
    // find how many bytes are in the buffer
    uint16_t len = strlen((char *)net_rx_buffer);
    if (len == 0)
    {
        return 0;
    }
    if (len > maxlen)
    {
        len = maxlen;
    }
    // copy them out
    memcpy(buf, net_rx_buffer, len);
    // clear the internal buffer so next receive starts fresh
    memset(net_rx_buffer, 0, sizeof(net_rx_buffer));
    return len;
}
