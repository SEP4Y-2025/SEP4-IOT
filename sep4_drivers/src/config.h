#ifndef MQTT_CONTROLLER_CONFIG_H
#define MQTT_CONTROLLER_CONFIG_H

#include <stdint.h>

/**
 * Configuration for the MQTT controller.
 * Used by mqtt_service and by your application.
 */
#ifndef MQTT_CONTROLLER_CONFIG_T_DEFINED
#define MQTT_CONTROLLER_CONFIG_T_DEFINED
typedef struct
{
    const char *client_id;       ///< MQTT client identifier
    uint16_t keepalive_interval; ///< Keep-alive in seconds
    uint8_t cleansession;        ///< Clean session flag (1 = true)
    uint8_t MQTTVersion;         ///< MQTT protocol level (4 = 3.1.1)
    const char *username;        ///< Username (or "" if none)
    const char *password;        ///< Password (or "" if none)
} mqtt_controller_config_t;
#endif

/**
 * Your device’s static configuration instance.
 * The `static` gives it internal linkage so each .c sees its own copy.
 */
static const mqtt_controller_config_t mqtt_cfg = {
    .client_id = "mega_iot_device",
    .keepalive_interval = 60,
    .cleansession = 1,
    .MQTTVersion = 4,
    .username = "",
    .password = "",
};

#endif // MQTT_CONTROLLER_CONFIG_H
