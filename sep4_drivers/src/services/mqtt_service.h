// mqtt_service.h
#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*mqtt_message_handler_t)(
    const char *topic,
    const uint8_t *payload,
    uint16_t len);

void mqtt_service_init(const char *broker_ip,
                       uint16_t broker_port,
                       const char *client_id);

void mqtt_service_poll(void);

// Register a handler for a topic
bool mqtt_service_subscribe(const char *topic, mqtt_message_handler_t handler);

// Send a message to a topic
bool mqtt_service_publish(const char *topic,
                          const uint8_t *payload,
                          uint16_t len);

#endif // MQTT_SERVICE_H
