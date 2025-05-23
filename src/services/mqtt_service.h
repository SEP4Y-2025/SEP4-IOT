#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <stdbool.h>
#include <stdint.h>
#include "wifi.h"       // For WIFI_ERROR_MESSAGE_t
#include "MQTTPacket.h" // For MQTTSerialize_*
#include "scheduler.h"  // For scheduler_millis()

// Buffer into which the network controller stuffs incoming TCP bytes
extern char callback_buff[256];

typedef enum
{
    MQTT_OK = 0,
    MQTT_FAIL = 1
} mqtt_error_t;

// Call once at startup to kick off the poller
void mqtt_service_init(const char *broker_ip, uint16_t broker_port);

// Called by network_controller when bytes arrive
void mqtt_service_event_callback(void);

WIFI_ERROR_MESSAGE_t mqtt_service_subscribe_to_topic(MQTTString topic);
// State queries / actions
bool mqtt_service_is_connected(void);
mqtt_error_t mqtt_service_connect(void);
mqtt_error_t mqtt_service_subscribe_to_all_topics(void);
mqtt_error_t mqtt_service_send_pingreq(void);
mqtt_error_t mqtt_service_publish(const char *topic, const char *payload);

size_t mqtt_service_create_mqtt_connect_packet(unsigned char *buf, size_t buflen);

void mqtt_service_poll(void);

#endif // MQTT_SERVICE_H
