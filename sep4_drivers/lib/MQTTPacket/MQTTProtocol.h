/*
 * MQTTProtocol.h
 * Minimal definitions for MQTT control packet types
 */
#ifndef MQTT_PROTOCOL_H
#define MQTT_PROTOCOL_H

// MQTT Control Packet types (Fixed Header first byte high nibble)
#define MQTTCONNECT       1
#define MQTTCONNACK       2
#define MQTTPUBLISH       3
#define MQTTPUBACK        4
#define MQTTPUBREC        5
#define MQTTPUBREL        6
#define MQTTPUBCOMP       7
#define MQTTSUBSCRIBE     8
#define MQTTSUBACK        9
#define MQTTUNSUBSCRIBE  10
#define MQTTUNSUBACK     11
#define MQTTPINGREQ      12
#define MQTTPINGRESP     13
#define MQTTDISCONNECT   14

// Aliases for readability
#define CONNECT     MQTTCONNECT
#define CONNACK     MQTTCONNACK
#define PUBLISH     MQTTPUBLISH
#define PUBACK      MQTTPUBACK
#define PUBREC      MQTTPUBREC
#define PUBREL      MQTTPUBREL
#define PUBCOMP     MQTTPUBCOMP
#define SUBSCRIBE   MQTTSUBSCRIBE
#define SUBACK      MQTTSUBACK
#define UNSUBSCRIBE MQTTUNSUBSCRIBE
#define UNSUBACK    MQTTUNSUBACK
#define PINGREQ     MQTTPINGREQ
#define PINGRESP    MQTTPINGRESP
#define DISCONNECT  MQTTDISCONNECT

// Legacy aliases for code using MQTT_* prefix
#define MQTT_PUBLISH   MQTTPUBLISH
#define MQTT_PINGRESP  MQTTPINGRESP

#endif // MQTT_PROTOCOL_H