// #ifndef MQTT_SERVICE_H
// #define MQTT_SERVICE_H

// #include <stdint.h>
// #include <stdbool.h>
// #include "controllers/mqtt_controller.h"
// #include "controllers/mqtt_client.h"

// /**
//  * Initialize the MQTT sub‐system with broker parameters and client config.
//  * Must be called exactly once at startup, before any publish or subscribe calls.
//  */
// void mqtt_service_init(const char *broker_ip,
//                        uint16_t      broker_port,
//                        const mqtt_controller_config_t *cfg);

// /**
//  * Non‐blocking: if not already connected, open TCP + send CONNECT.
//  * Returns true once the MQTT session is up (CONNACK received).
//  */
// bool mqtt_service_connect(void);

// /**
//  * Publish a QoS0 / no‐retain message.
//  * Returns true on successful send.
//  */
// bool mqtt_service_publish(const char *topic,
//                           const uint8_t *payload,
//                           uint16_t len);

// /**
//  * Poll for incoming packets (CONNACK, PUBLISH, etc.).
//  * Call this regularly from your main loop.
//  */
// void mqtt_service_poll(void);

// /**
//  * Register a handler for incoming PUBLISH messages.
//  */
// void mqtt_service_set_message_callback(mqtt_message_cb_t cb);

// /**
//  * Subscribe to a topic (QoS0).
//  * Returns true if the SUBSCRIBE was sent (and SUBACK received).
//  */
// bool mqtt_service_subscribe(const char *topic);

// #endif // MQTT_SERVICE_H

/*
 * mqtt_service.h
 * Minimal MQTT client on top of network_controller (ESP8266 AT)
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "controllers/network_controller.h"
#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Callback for incoming MQTT messages.
     * @param topic     Null-terminated topic string.
     * @param payload   Pointer to payload bytes.
     * @param length    Number of payload bytes.
     */
    typedef void (*mqtt_message_callback_t)(const char *topic, const uint8_t *payload, uint16_t length);

    /**
     * Initialize MQTT service with broker address and register message callback.
     * Must be called once after wifi_service_is_connected() == true.
     * @param broker_ip   ASCII broker IP (e.g. "192.168.1.100").
     * @param port        MQTT port, usually 1883.
     * @param cb          Callback for received messages.
     * @return true on success (TCP opened+callback set), false otherwise.
     */
    /** Initialize: open TCP, set callback, and send CONNECT using the given config. */
    bool mqtt_service_init(const char *broker_ip,
                           uint16_t port,
                           const mqtt_controller_config_t *cfg,
                           mqtt_message_callback_t cb);
    /**
     * Send MQTT CONNECT (clean session) with optional user/pw.
     * @param client_id    Null-terminated client identifier.
     * @param username     Optional, or NULL.
     * @param password     Optional, or NULL.
     * @param keep_alive_s Keep-alive in seconds.
     * @return true if CONNECT packet sent, false otherwise.
     */
    bool mqtt_service_connect(const char *client_id,
                              const char *username,
                              const char *password,
                              uint16_t keep_alive_s);

    /**
     * Publish a message to a topic.
     * @param topic       Null-terminated topic string.
     * @param payload     Pointer to payload bytes.
     * @param payload_len Number of payload bytes.
     * @param qos         Quality of Service (0 or 1).
     * @param retain      Retain flag.
     * @return true if PUBLISH packet sent, false otherwise.
     */
    bool mqtt_service_publish(const char *topic,
                              const uint8_t *payload,
                              uint16_t payload_len,
                              uint8_t qos,
                              bool retain);

    /**
     * Subscribe to a topic.
     * @param topic Topic filter (e.g. "sensors/temperature").
     * @param qos   Requested QoS (0 or 1).
     * @return true if SUBSCRIBE sent, false otherwise.
     */
    bool mqtt_service_subscribe(const char *topic, uint8_t qos);

    /**
     * Call periodically (e.g., from main loop) to process incoming packets.
     */
    void mqtt_service_loop(void);

#ifdef __cplusplus
}
#endif