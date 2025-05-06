// mqtt_controller.c
#include "mqtt_controller.h"
#include "MQTTPacket.h"
#include <string.h>
#include <stdbool.h>

static mqtt_controller_config_t _mqtt_cfg;
static mqtt_connack_callback_t connack_callback = NULL;

void mqtt_controller_init(const mqtt_controller_config_t *config)
{
    // Copy user-supplied config into internal storage
    memcpy(&_mqtt_cfg, config, sizeof(_mqtt_cfg));
}

size_t mqtt_controller_build_connect_packet(
    unsigned char *buf,
    size_t buflen)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    // Populate the fields from stored config
    data.clientID.cstring = (char *)_mqtt_cfg.client_id;
    data.keepAliveInterval = _mqtt_cfg.keepalive_interval;
    data.cleansession = _mqtt_cfg.cleansession;
    data.MQTTVersion = _mqtt_cfg.MQTTVersion;

    // Optionally set username/password
    if (_mqtt_cfg.username)
    {
        data.username.cstring = (char *)_mqtt_cfg.username;
    }
    if (_mqtt_cfg.password)
    {
        data.password.cstring = (char *)_mqtt_cfg.password;
    }

    return MQTTSerialize_connect(buf, buflen, &data);
}

size_t mqtt_controller_build_publish_packet(
    const char *topic,
    const unsigned char *payload,
    size_t payloadlen,
    unsigned char *buf,
    size_t buflen)
{
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = (char *)topic;

    // Flags: dup=0, qos=0, retained=0, packetid=0 (QoS0)
    return MQTTSerialize_publish(
        buf, buflen,
        0, 0, 0, 0,
        topicString,
        (char *)payload,
        payloadlen);
}

size_t mqtt_controller_build_disconnect_packet(
    unsigned char *buf,
    size_t buflen)
{
    return MQTTSerialize_disconnect(buf, buflen);
}

size_t mqtt_controller_build_ping_packet(unsigned char *buf, size_t bufsize)
{
    if (bufsize < 2)
        return 0;
    return MQTTSerialize_pingreq(buf, bufsize);
}

bool mqtt_controller_handle_connack(const unsigned char *buf, size_t buflen)
{
    unsigned char sessionPresent;
    unsigned char connack_rc;

    int rc = MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen);

    if (rc != 1)
    {
        logger_service_log("Failed to deserialize CONNACK packet");
        return false;
    }

    if (connack_rc != 0)
    {
        logger_service_log("CONNACK returned error code: %d", connack_rc);
        return false;
    }

    logger_service_log("MQTT connection acknowledged (CONNACK)");

    // Call back to mqtt_service
    if (connack_callback)
    {
        connack_callback();
    }

    return true;
}

void mqtt_controller_set_connack_callback(mqtt_connack_callback_t cb)
{
    connack_callback = cb;
}
