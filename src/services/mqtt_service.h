#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <stddef.h>
#include "wifi.h"
#include "MQTTPacket.h"

extern char callback_buff[256];

void mqtt_service_event_callback(void);
void  mqtt_service_send_pingreq(void);
void mqtt_service_subscribe_to_all_topics(void);
size_t mqtt_service_create_mqtt_connect_packet(unsigned char *buf, size_t buflen);
WIFI_ERROR_MESSAGE_t mqtt_service_publish(char *topic, unsigned char *payload,
                                   unsigned char *buf, size_t buflen);
size_t mqtt_service_create_mqtt_disconnect_packet(unsigned char *buf, size_t buflen);
WIFI_ERROR_MESSAGE_t mqtt_service_subscribe_to_topic(  MQTTString topic);

#endif
