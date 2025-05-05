// #include "services/mqtt_service.h"
// #include "controllers/network_controller.h"
// #include "controllers/mqtt_controller.h"
// #include "controllers/mqtt_client.h" // your client‐layer we sketched earlier
// #include "services/wifi_service.h"
// #include "services/logger_service.h"
// #include "scheduler.h"

// static const char *_svc_broker_ip;
// static uint16_t _svc_broker_port;
// static bool _connected = false;

// void mqtt_service_init(const char *broker_ip,
//                        uint16_t broker_port,
//                        const mqtt_controller_config_t *cfg)
// {
//     _svc_broker_ip = broker_ip;
//     _svc_broker_port = broker_port;
//     mqtt_controller_init(cfg);
//     mqtt_client_init(cfg);
//     logger_service_log("MQTT Service: init for %s:%u",
//                        broker_ip, broker_port);
// }

// bool mqtt_service_connect(void)
// {
//     if (_connected)
//         return true;
//     if (!wifi_service_is_connected())
//     {
//         logger_service_log("MQTT Service: Wi‑Fi not yet connected");
//         _connected = false;
//         return false;
//     }
//     logger_service_log("MQTT Service: opening TCP + MQTT connect…");
//     // if (!network_controller_tcp_open(_svc_broker_ip, _svc_broker_port))
//     // {
//     //     logger_service_log("MQTT Service: TCP open failed");
//     //     return false;
//     // }
//     if (!mqtt_client_connect(_svc_broker_ip, _svc_broker_port))
//     {
//         logger_service_log("MQTT Service: MQTT connect failed");
//         _connected = false;
//         return false;
//     }
//     _connected = true;
//     logger_service_log("MQTT Service: connected!");
//     return true;
// }

// bool mqtt_service_publish(const char *topic, const uint8_t *payload, uint16_t len)
// {
//     if (!_connected)
//     {
//         logger_service_log("MQTT Service: publish failed, not connected");
//         return false;
//     }
//     logger_service_log("MQTT Service: publishing to '%s' (%u bytes)", topic, len);
//     return mqtt_client_publish(topic, payload, len);
// }

// void mqtt_service_poll(void)
// {
//     if (_connected)
//     {
//         mqtt_client_poll();
//     }
// }

// void mqtt_service_set_message_callback(mqtt_message_cb_t cb)
// {
//     logger_service_log("MQTT Service: set message callback");
//     mqtt_client_set_message_callback(cb);
// }

// bool mqtt_service_subscribe(const char *topic)
// {
//     logger_service_log("MQTT Service: subscribing to '%s'", topic);
//     if (!mqtt_service_connect())
//         return false;
//     return mqtt_client_subscribe(topic);
// }

/*
 * mqtt_service.c
 * Implements minimal MQTT client over network_controller.
 */
#include "services/mqtt_service.h"
#include "services/logger_service.h"
#include <string.h>

#define MQTT_MAX_PACKET 256
static char _broker_ip[16];
static uint16_t _broker_port;
static mqtt_message_callback_t _msg_cb;

// State for incoming packet parsing
static uint8_t _in_buf[512];
static uint16_t _in_len = 0;
static uint32_t _bytes_needed = 0;
static uint8_t _remain_multiplier;
static uint32_t _remain_length;
static const mqtt_controller_config_t *_cfg;

// Internal TCP receive callback
static void _net_tcp_cb(const uint8_t *data, uint16_t len)
{
    // Append to buffer
    if (_in_len + len > sizeof(_in_buf))
        len = sizeof(_in_buf) - _in_len;
    memcpy(_in_buf + _in_len, data, len);
    _in_len += len;
}

static bool _encode_length(uint8_t *buf, uint32_t length, uint16_t *pos_out)
{
    uint16_t pos = 0;
    do
    {
        uint8_t byte = length % 128;
        length /= 128;
        if (length > 0)
            byte |= 0x80;
        buf[pos++] = byte;
    } while (length);
    *pos_out = pos;
    return true;
}

static uint16_t _str_write(uint8_t *buf, const char *s)
{
    uint16_t len = strlen(s);
    buf[0] = (len >> 8) & 0xFF;
    buf[1] = len & 0xFF;
    memcpy(buf + 2, s, len);
    return len + 2;
}

bool mqtt_service_init(const char *broker_ip,
                       uint16_t port,
                       const mqtt_controller_config_t *cfg,
                       mqtt_message_callback_t cb)
{
    // stash the config and callback
    _cfg = cfg;
    _msg_cb = cb;

    // open TCP
    strncpy(_broker_ip, broker_ip, sizeof(_broker_ip) - 1);
    _broker_ip[sizeof(_broker_ip) - 1] = 0;
    _broker_port = port;
    if (!network_controller_tcp_open(_broker_ip, _broker_port))
    {
        logger_service_log("MQTT: TCP open failed");
        return false;
    }
    network_controller_set_tcp_callback(_net_tcp_cb);

    // immediately send CONNECT using all fields in cfg
    return mqtt_service_connect(
        _cfg->client_id,
        _cfg->username,
        _cfg->password,
        _cfg->keepalive_interval);
}

bool mqtt_service_connect(const char *client_id,
                          const char *username,
                          const char *password,
                          uint16_t keep_alive_s)
{
    uint8_t packet[MQTT_MAX_PACKET];
    uint16_t pos = 0;
    // Fixed header
    packet[pos++] = 0x10; // CONNECT
    // Reserve space for remaining length (max 4 bytes)
    uint8_t rl_buf[4];
    uint16_t rl_len;
    // Variable header + payload sizes
    uint16_t vh_pl_len = 2 + 4 + 1 + 1 + 2 + /*client id*/ (2 + strlen(client_id));
    if (username)
        vh_pl_len += 2 + strlen(username);
    if (password)
        vh_pl_len += 2 + strlen(password);
    _encode_length(rl_buf, vh_pl_len, &rl_len);
    memcpy(packet + pos, rl_buf, rl_len);
    pos += rl_len;

    // Protocol name
    pos += _str_write(packet + pos, "MQTT");
    // Protocol level
    packet[pos++] = 0x04;
    // Connect flags
    uint8_t flags = 0;
    if (username)
        flags |= 0x80;
    if (password)
        flags |= 0x40;
    flags |= 0x02; // clean session
    packet[pos++] = flags;
    // Keep alive
    packet[pos++] = (keep_alive_s >> 8) & 0xFF;
    packet[pos++] = keep_alive_s & 0xFF;
    // Payload: client id
    pos += _str_write(packet + pos, client_id);
    // Username & password
    if (username)
        pos += _str_write(packet + pos, username);
    if (password)
        pos += _str_write(packet + pos, password);

    return network_controller_tcp_send(packet, pos);
}

bool mqtt_service_publish(const char *topic,
                          const uint8_t *payload,
                          uint16_t payload_len,
                          uint8_t qos,
                          bool retain)
{
    uint8_t packet[MQTT_MAX_PACKET];
    uint16_t pos = 0;
    // Fixed header: PUBLISH (0x30 | flags)
    uint8_t flags = (retain ? 1 : 0) | ((qos & 1) << 1);
    packet[pos++] = 0x30 | flags;
    // Remaining length = topic len + 2 + payload_len (+2 for packet id if QoS1)
    uint32_t rem_len = 2 + strlen(topic) + payload_len + (qos ? 2 : 0);
    uint8_t rl_buf[4];
    uint16_t rl_len;
    _encode_length(rl_buf, rem_len, &rl_len);
    memcpy(packet + pos, rl_buf, rl_len);
    pos += rl_len;
    // Topic
    pos += _str_write(packet + pos, topic);
    // Packet ID if QoS1
    if (qos)
    {
        packet[pos++] = 0x00;
        packet[pos++] = 0x01; // simple fixed ID=1
    }
    // Payload
    memcpy(packet + pos, payload, payload_len);
    pos += payload_len;

    return network_controller_tcp_send(packet, pos);
}

bool mqtt_service_subscribe(const char *topic, uint8_t qos)
{
    uint8_t packet[MQTT_MAX_PACKET];
    uint16_t pos = 0;
    packet[pos++] = 0x82; // SUBSCRIBE with QoS1
    // Remaining length = 2 (packet id) + 2 + topic len + 1
    uint32_t rem_len = 2 + 2 + strlen(topic) + 1;
    uint8_t rl_buf[4];
    uint16_t rl_len;
    _encode_length(rl_buf, rem_len, &rl_len);
    memcpy(packet + pos, rl_buf, rl_len);
    pos += rl_len;
    // Packet ID
    packet[pos++] = 0x00;
    packet[pos++] = 0x01;
    // Topic filter
    pos += _str_write(packet + pos, topic);
    // Requested QoS
    packet[pos++] = qos & 1;

    return network_controller_tcp_send(packet, pos);
}

void mqtt_service_loop(void)
{
    if (_in_len < 2)
        return;
    // Parse fixed header
    uint8_t type = _in_buf[0] >> 4;
    // Decode remaining length
    uint32_t multiplier = 1;
    uint32_t value = 0;
    uint16_t idx = 1;
    uint8_t encoded;
    do
    {
        encoded = _in_buf[idx++];
        value += (encoded & 127) * multiplier;
        multiplier *= 128;
    } while ((encoded & 128) != 0 && idx < _in_len);
    if (_in_len < idx + value)
        return; // wait for full packet
    // Handle PUBLISH
    if (type == 3)
    {
        // Topic length
        uint16_t topic_len = (_in_buf[idx] << 8) | _in_buf[idx + 1];
        idx += 2;
        char topic[64];
        if (topic_len >= sizeof(topic))
            topic_len = sizeof(topic) - 1;
        memcpy(topic, _in_buf + idx, topic_len);
        topic[topic_len] = '\0';
        idx += topic_len;
        // QoS1 packet id
        uint8_t qos = ((_in_buf[0] & 0x06) >> 1);
        if (qos)
            idx += 2;
        uint16_t payload_len = value - (idx - 1);
        if (_msg_cb)
        {
            _msg_cb(topic, _in_buf + idx, payload_len);
        }
    }
    // Remove processed packet
    uint16_t total = idx + value;
    memmove(_in_buf, _in_buf + total, _in_len - total);
    _in_len -= total;
}
