// src/controllers/network_controller.h
#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "wifi.h"

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

// /**
//  * @brief Join the given WiFi network (blocking).
//  * @param ssid     Null-terminated SSID string.
//  * @param password Null-terminated password string.
//  * @return true on success, false on failure.
//  */
// bool network_controller_connect_ap(const char *ssid, const char *password);

WIFI_ERROR_MESSAGE_t network_controller_connect_ap(char *ssid, char *password, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer);

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

#endif // NETWORK_CONTROLLER_H
