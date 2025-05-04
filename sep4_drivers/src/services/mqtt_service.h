#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "controllers/mqtt_controller.h"
#include "controllers/mqtt_client.h"

/**  
 * Initialize the MQTT sub‐system with broker parameters and client config.  
 * Must be called exactly once at startup, before any publish or subscribe calls.  
 */
void mqtt_service_init(const char *broker_ip,
                       uint16_t      broker_port,
                       const mqtt_controller_config_t *cfg);

/**  
 * Non‐blocking: if not already connected, open TCP + send CONNECT.  
 * Returns true once the MQTT session is up (CONNACK received).  
 */
bool mqtt_service_connect(void);

/**  
 * Publish a QoS0 / no‐retain message.  
 * Returns true on successful send.  
 */
bool mqtt_service_publish(const char *topic,
                          const uint8_t *payload,
                          uint16_t len);

/**  
 * Poll for incoming packets (CONNACK, PUBLISH, etc.).  
 * Call this regularly from your main loop.  
 */
void mqtt_service_poll(void);

/**  
 * Register a handler for incoming PUBLISH messages.  
 */
void mqtt_service_set_message_callback(mqtt_message_cb_t cb);

/**
 * Subscribe to a topic (QoS0).  
 * Returns true if the SUBSCRIBE was sent (and SUBACK received).
 */
bool mqtt_service_subscribe(const char *topic);


#endif // MQTT_SERVICE_H
