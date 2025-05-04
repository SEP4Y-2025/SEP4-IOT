// src/controllers/network_controller.h
#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char   ssid[33];   ///< Null-terminated SSID
    int16_t rssi;      ///< RSSI in dBm
} wifi_ap_t;

typedef struct {
    const char *ssid;      ///< Null-terminated SSID
    const char *password;  ///< Null-terminated password
} wifi_credential_t;

/**
 * @brief Callback invoked when TCP data arrives.
 * @param data Pointer to buffer holding the received bytes.
 * @param len  Number of bytes in the buffer.
 */
typedef void (*Network_TCP_Callback_t)(const uint8_t *data, uint16_t len);

/**
 * @brief Initialize the underlying UART & WiFi module.
 *        Must be called once at startup.
 */
void network_controller_init(void);

/**
 * @brief Perform one-time setup commands (disable echo, set mode, single-conn).
 * @return true on success, false otherwise.
 */
bool network_controller_setup(void);

/**
 * @brief Join the given WiFi network (blocking).
 * @param ssid     Null-terminated SSID string.
 * @param password Null-terminated password string.
 * @return true on success, false on failure.
 */
bool network_controller_connect_ap(const char *ssid, const char *password);

/**
 * @brief Leave the current WiFi network.
 * @return true on success.
 */
bool network_controller_disconnect_ap(void);

/**
 * @brief Check if the WiFi module is responsive (and thus likely still on-line).
 * @return true if it replies “OK” to AT.
 */
bool network_controller_is_ap_connected(void);

/**
 * @brief Open a TCP connection to the given IP:port.
 * @param ip   Null-terminated ASCII IP (e.g. "192.168.1.42").
 * @param port TCP port number.
 * @return true on success.
 */
bool network_controller_tcp_open(const char *ip, uint16_t port);

/**
 * @brief Register your own callback for incoming TCP data.
 */
void network_controller_set_tcp_callback(Network_TCP_Callback_t cb);

/**
 * @brief Send raw bytes over the open TCP connection.
 */
bool network_controller_tcp_send(const uint8_t *data, uint16_t len);

/**
 * @brief Close the TCP connection.
 */
bool network_controller_tcp_close(void);

/**
 * @brief Scan for up to max_aps access points, filling the list.
 * @param list      Caller-allocated array of length max_aps
 * @param max_aps   Maximum entries to parse
 * @param timeout_s How many seconds to wait for AT+CWLAP
 * @return Number of entries actually parsed (≤ max_aps)
 */
uint8_t network_controller_scan_aps(wifi_ap_t *list,
    uint8_t    max_aps,
    uint16_t   timeout_s);

/**
* @brief Given a compile-time list of known SSID/password pairs, scan for
*        the strongest available and connect to it.
*
* @param known     Array of wifi_credentials_t pairs
* @param known_len Number of entries in known[]
* @param scan_timeout_s Timeout for the CWLAP scan
* @return true on successful connect, false otherwise
*/

bool network_controller_connect_best(const wifi_credential_t *known, uint8_t known_len, uint16_t scan_timeout_s);

/**
 * @brief Read up to `maxlen` bytes from the open TCP connection.
 * @param buf    Pointer to a buffer to fill.
 * @param maxlen Maximum bytes to read.
 * @return Number of bytes actually read, or -1 on error.
 */
int  network_controller_tcp_receive(uint8_t *buf, uint16_t maxlen);


#endif // NETWORK_CONTROLLER_H
