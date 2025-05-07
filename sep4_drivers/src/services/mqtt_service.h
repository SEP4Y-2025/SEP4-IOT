// mqtt_service.h
#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <stddef.h>
#include "wifi.h"
#include "MQTTPacket.h"


size_t create_mqtt_connect_packet(unsigned char *buf, size_t buflen);
size_t create_mqtt_transmit_packet(char *topic, unsigned char *payload,
                                   unsigned char *buf, size_t buflen);
size_t create_mqtt_disconnect_packet(unsigned char *buf, size_t buflen);
WIFI_ERROR_MESSAGE_t mqtt_subscribe_to_topic(  MQTTString topic);

#endif
