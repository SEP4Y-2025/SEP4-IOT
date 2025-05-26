#include "controllers/network_controller.h"
#include "services/logger_service.h"
#include "services/mqtt_service.h"

// #include "services/wifi_service.h"
#include "wifi.h"
#include "config/wifi_credentials.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/// Store user’s TCP callback and internal buffer
static Network_TCP_Callback_t user_cb = NULL;
#define NET_TCP_BUFFER_SIZE 512
static uint8_t net_rx_buffer[NET_TCP_BUFFER_SIZE];
static bool _tcp_connected = false;

static void (*_tcp_cb)(void);
static char *_tcp_buf;

// From config/wifi_credentials.h
bool wifi_get_best_credentials(uint16_t timeout_s,
                               char *out_ssid,
                               char *out_password)
{
    // 0) dump known list
    LOG("get_best_creds: %u known SSIDs:", known_credentials_len);
    for (uint8_t k = 0; k < known_credentials_len; k++)
    {
        LOG("  [%u] \"%s\"", k, known_credentials[k].ssid);
    }

    // 1) scan
    LOG("get_best_creds: scanning (timeout=%us)...", timeout_s);
    if (wifi_scan_APs(timeout_s) != WIFI_OK)
    {
        LOG("get_best_creds: scan failed");
        return false;
    }

    const char *raw = wifi_get_scan_buffer();
    uint16_t rawlen = wifi_get_scan_buffer_len();
    if (rawlen == 0)
    {
        LOG("get_best_creds: no scan data");
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
                LOG("  found [%u] '%s' (RSSI %d)",
                    found_count, a.ssid, a.rssi);
                found[found_count++] = a;
            }
        }
        line = strtok_r(NULL, "\r\n", &saveptr);
    }
    free(copy);

    if (found_count == 0)
    {
        LOG("get_best_creds: no APs parsed");
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
        LOG("get_best_creds: no known SSIDs found");
        return false;
    }

    // 4) copy out the winner
    strcpy(out_ssid, known_credentials[best_idx].ssid);
    strcpy(out_password, known_credentials[best_idx].password);
    LOG("get_best_creds: chosen '%s' (%d dBm)",
        out_ssid, best_rssi);

    return true;
}

void network_controller_init(void (*tcp_evt_cb)(void), char *tcp_evt_buf)
{
    _tcp_cb = tcp_evt_cb;
    _tcp_buf = tcp_evt_buf;
    _tcp_connected = false;
    // init UART & WiFi module
    wifi_init(); // :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}
    network_controller_setup();
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

WIFI_ERROR_MESSAGE_t network_controller_connect_ap(uint16_t timeout_s)
{
    char ssid[33], pwd[64];
    if (!wifi_get_best_credentials(timeout_s, ssid, pwd))
        return WIFI_FAIL;

    if (wifi_command_join_AP(ssid, pwd) != WIFI_OK)
    {
        LOG("Wi-Fi: join AP failed\n");
        return WIFI_FAIL;
    }
    LOG("Wi-Fi: joined AP\n");
    return WIFI_OK;
}

WIFI_ERROR_MESSAGE_t network_controller_disconnect_ap(void)
{
    WIFI_ERROR_MESSAGE_t res = wifi_command_quit_AP();
    if (res != WIFI_OK)
    {
        LOG("Wi-Fi quit AP failed: error=%d\n", res);
    }
    return res;
}

bool network_controller_is_tcp_connected(void)
{
    return _tcp_connected;
}

bool network_controller_is_ap_connected(void)
{
    if (wifi_command_CWJAP_status() != WIFI_OK)
    {
        return false;
    }

    const char *buf = wifi_get_scan_buffer();
    return (strstr(buf, "+CWJAP:") != NULL);
}

WIFI_ERROR_MESSAGE_t network_controller_tcp_open(const char *ip,
                                                 uint16_t port)
{
    WIFI_ERROR_MESSAGE_t r = wifi_command_create_TCP_connection(
        (char *)ip, port,
        _tcp_cb, _tcp_buf);
    _tcp_connected = (r == WIFI_OK);
    LOG("TCP open %s:%u → %s (err=%d)\n",
        ip, (unsigned)port,
        r == WIFI_OK ? "OK" : "FAIL",
        r);
    return r;
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
