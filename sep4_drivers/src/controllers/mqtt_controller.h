// mqtt_controller.h
#ifndef MQTT_CONTROLLER_H
#define MQTT_CONTROLLER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Configuration for MQTT connection parameters.
 */
typedef struct {
    const char     *client_id;          ///< MQTT client identifier (NULL-terminated)
    uint16_t        keepalive_interval; ///< Keep-alive interval in seconds
    uint8_t         cleansession;       ///< 1 = clean session, 0 = resume session
    uint8_t         MQTTVersion;        ///< Protocol level (e.g. 4 for 3.1.1)
    const char     *username;           ///< Username (or NULL if none)
    const char     *password;           ///< Password (or NULL if none)
} mqtt_controller_config_t;

/**
 * Initialize the MQTT controller with the given parameters.
 * Must be called before building any packets.
 */
void mqtt_controller_init(const mqtt_controller_config_t *config);

/**
 * Build an MQTT CONNECT packet into buf.
 * @return Number of bytes written, or 0 on error.
 */
size_t mqtt_controller_build_connect_packet(
    unsigned char *buf,
    size_t         buflen
);

/**
 * Build an MQTT PUBLISH packet (QoS0, no retain) into buf.
 * @param topic       Topic string (NULL-terminated)
 * @param payload     Pointer to payload bytes
 * @param payloadlen  Length of payload in bytes
 * @return Number of bytes written, or 0 on error.
 */
size_t mqtt_controller_build_publish_packet(
    const char    *topic,
    const unsigned char *payload,
    size_t         payloadlen,
    unsigned char *buf,
    size_t         buflen
);

/**
 * Build an MQTT DISCONNECT packet into buf.
 * @return Number of bytes written (always >0), or 0 on error.
 */
size_t mqtt_controller_build_disconnect_packet(
    unsigned char *buf,
    size_t         buflen
);

size_t mqtt_controller_build_ping_packet(unsigned char *buf, size_t bufsize);

bool mqtt_controller_handle_connack(const unsigned char *buf, size_t buflen);

typedef void (*mqtt_connack_callback_t)(void);
void mqtt_controller_set_connack_callback(mqtt_connack_callback_t cb);

#endif // MQTT_CONTROLLER_H