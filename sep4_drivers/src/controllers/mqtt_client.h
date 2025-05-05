#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H
#include "mqtt_controller.h"

#include <stdint.h>
#include <stdbool.h>

/** Called whenever a PUBLISH arrives */
typedef void (*mqtt_message_cb_t)(const char *topic,
                                  const uint8_t *payload,
                                  uint16_t len);

/**
 * Initialize the client with your packet-builder config.
 * Must be called before connect().
 */
void mqtt_client_init(const mqtt_controller_config_t *cfg);

/**
 * Open TCP, send CONNECT, wait for CONNACK.
 * Returns true on successful CONNACK.
 */
bool mqtt_client_connect(const char *broker_ip,
                         uint16_t broker_port);

/**
 * Subscribe to a topic (QoS0).  Returns true on SUBACK.
 * You’ll need to add mqtt_controller_build_subscribe_packet().
 */
bool mqtt_client_subscribe(const char *topic);

/**
 * Publish at QoS0.  Returns true if send succeeded.
 */
bool mqtt_client_publish(const char *topic,
                         const uint8_t *payload,
                         uint16_t len);

/**
 * Poll for inbound packets (CONNACK, PUBLISH, PINGRESP, etc.)
 * Must be called regularly inside your main loop.
 */
void mqtt_client_poll(void);

/**
 * Register a callback for incoming PUBLISH messages.
 */
void mqtt_client_set_message_callback(mqtt_message_cb_t cb);

/**
 * Gracefully disconnect (send DISCONNECT + close socket).
 */
void mqtt_client_disconnect(void);

#endif // MQTT_CLIENT_H
