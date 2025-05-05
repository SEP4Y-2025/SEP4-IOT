// mqtt_controller.c
#include "mqtt_controller.h"
#include "MQTTPacket.h"
#include <string.h>
#include "services/logger_service.h"

static mqtt_controller_config_t _mqtt_cfg;

void mqtt_controller_init(const mqtt_controller_config_t *config)
{
    // Copy user-supplied config into internal storage
    memcpy(&_mqtt_cfg, config, sizeof(_mqtt_cfg));
    logger_service_log("MQTT Ctrl: init client_id=%s keepalive=%u clean=%u version=%u",
                       _mqtt_cfg.client_id,
                       _mqtt_cfg.keepalive_interval,
                       _mqtt_cfg.cleansession,
                       _mqtt_cfg.MQTTVersion);
}

size_t mqtt_controller_build_connect_packet(unsigned char *buf, size_t buflen)
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = (char *)_mqtt_cfg.client_id;
    data.keepAliveInterval = _mqtt_cfg.keepalive_interval;
    data.cleansession = _mqtt_cfg.cleansession;
    data.MQTTVersion = _mqtt_cfg.MQTTVersion;
    if (_mqtt_cfg.username)
        data.username.cstring = (char *)_mqtt_cfg.username;
    if (_mqtt_cfg.password)
        data.password.cstring = (char *)_mqtt_cfg.password;

    size_t len = MQTTSerialize_connect(buf, buflen, &data);
    logger_service_log("MQTT Ctrl: CONNECT packet built (%u bytes)", (unsigned)len);
    return len;
}

size_t mqtt_controller_build_publish_packet(
    const char *topic,
    const unsigned char *payload,
    size_t payloadlen,
    unsigned char *buf,
    size_t buflen)
{
    MQTTString t = MQTTString_initializer;
    t.cstring = (char *)topic;
    size_t len = MQTTSerialize_publish(
        buf, buflen,
        0, 0, 0, 0,
        t,
        (char *)payload,
        payloadlen);
    logger_service_log("MQTT Ctrl: PUBLISH '%s' (%u bytes payload) → packet %u bytes",
                       topic, (unsigned)payloadlen, (unsigned)len);
    return len;
}

size_t mqtt_controller_build_disconnect_packet(
    unsigned char *buf,
    size_t buflen)
{
    return MQTTSerialize_disconnect(buf, buflen);
}
