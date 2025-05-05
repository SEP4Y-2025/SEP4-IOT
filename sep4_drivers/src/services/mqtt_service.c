#include "services/mqtt_service.h"
#include "controllers/network_controller.h"
#include "controllers/mqtt_controller.h"
#include "controllers/mqtt_client.h" // your client‐layer we sketched earlier
#include "services/wifi_service.h"
#include "services/logger_service.h"
#include "scheduler.h"

static const char *_svc_broker_ip;
static uint16_t _svc_broker_port;
static bool _connected = false;

void mqtt_service_init(const char *broker_ip,
                       uint16_t broker_port,
                       const mqtt_controller_config_t *cfg)
{
    _svc_broker_ip = broker_ip;
    _svc_broker_port = broker_port;
    mqtt_controller_init(cfg);
    mqtt_client_init(cfg);
    logger_service_log("MQTT Service: init for %s:%u",
                       broker_ip, broker_port);
}

bool mqtt_service_connect(void)
{
    if (_connected)
        return true;
    if (!wifi_service_is_connected())
    {
        logger_service_log("MQTT Service: Wi‑Fi not yet connected");
        _connected = false;
        return false;
    }
    logger_service_log("MQTT Service: opening TCP + MQTT connect…");
    // if (!network_controller_tcp_open(_svc_broker_ip, _svc_broker_port))
    // {
    //     logger_service_log("MQTT Service: TCP open failed");
    //     return false;
    // }
    if (!mqtt_client_connect(_svc_broker_ip, _svc_broker_port))
    {
        logger_service_log("MQTT Service: MQTT connect failed");
        _connected = false;
        return false;
    }
    _connected = true;
    logger_service_log("MQTT Service: connected!");
    return true;
}

bool mqtt_service_publish(const char *topic, const uint8_t *payload, uint16_t len)
{
    if (!_connected)
    {
        logger_service_log("MQTT Service: publish failed, not connected");
        return false;
    }
    logger_service_log("MQTT Service: publishing to '%s' (%u bytes)", topic, len);
    return mqtt_client_publish(topic, payload, len);
}

void mqtt_service_poll(void)
{
    if (_connected)
    {
        mqtt_client_poll();
    }
}

void mqtt_service_set_message_callback(mqtt_message_cb_t cb)
{
    logger_service_log("MQTT Service: set message callback");
    mqtt_client_set_message_callback(cb);
}

bool mqtt_service_subscribe(const char *topic)
{
    logger_service_log("MQTT Service: subscribing to '%s'", topic);
    if (!mqtt_service_connect())
        return false;
    return mqtt_client_subscribe(topic);
}
