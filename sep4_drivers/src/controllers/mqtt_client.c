#include "controllers/mqtt_client.h"
#include "controllers/mqtt_controller.h" // your packet builder
#include "controllers/network_controller.h"
#include "MQTTPacket.h" // for parsing
#include <string.h>
#include "mqtt_controller.h"
#include "MQTTProtocol.h"
#include "services/logger_service.h"

static mqtt_message_cb_t _msg_cb;
static mqtt_controller_config_t _pkt_cfg;

/** Helper: send a built packet and return success if network send ok */
static bool _send_packet(const unsigned char *pkt, size_t len)
{
    return network_controller_tcp_send(pkt, len);
}

void mqtt_client_init(const mqtt_controller_config_t *cfg)
{
    memcpy(&_pkt_cfg, cfg, sizeof(_pkt_cfg));
    mqtt_controller_init(cfg);
}

bool mqtt_client_connect(const char *broker_ip, uint16_t broker_port)
{
    // unsigned char buf[256];
    logger_service_log("MQTT Client: connecting to %s:%u",
                       broker_ip, broker_port);

    // 1) TCP open
    if (!network_controller_tcp_open(broker_ip, broker_port))
    {
        logger_service_log("MQTT Client: TCP open failed");
        return false;
    }

    return true;

    // // 2) CONNECT packet
    // size_t len = mqtt_controller_build_connect_packet(buf, sizeof(buf));
    // if (len == 0 || !network_controller_tcp_send(buf, len))
    // {
    //     logger_service_log("MQTT Client: failed to send CONNECT");
    //     return false;
    // }

    // // 3) Wait for CONNACK
    // int n = network_controller_tcp_receive(buf, sizeof(buf));
    // logger_service_log("MQTT Client: received %d bytes for CONNACK", n);
    // if (n <= 0)
    //     return false;

    // unsigned char rc1, rc2;
    // int rc = MQTTDeserialize_connack(&rc1, &rc2, buf, n);
    // logger_service_log("MQTT Client: CONNACK deserialize → %d, code=%u",
    //                    rc, rc2);
    // return (rc == 1 && rc2 == 0);
}

bool mqtt_client_subscribe(const char *topic)
{
    unsigned char buf[256];
    MQTTString tstr = MQTTString_initializer;
    tstr.cstring = (char *)topic;
    int len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, 1, 1, &tstr, NULL);
    logger_service_log("MQTT Client: SUBSCRIBE '%s' → packet %d bytes", topic, len);
    if (len <= 0 || !_send_packet(buf, len))
    {
        logger_service_log("MQTT Client: SUBSCRIBE send failed");
        return false;
    }
    // Wait for SUBACK
    int n = network_controller_tcp_receive(buf, sizeof(buf));
    logger_service_log("MQTT Client: received %d bytes for SUBACK", n);
    unsigned short pid;
    int count;
    int grantedQoSs[1];
    int rc = MQTTDeserialize_suback(&pid, 1, &count, grantedQoSs, buf, n);
    logger_service_log("MQTT Client: SUBACK rc=%d pid=%u qos0=%d",
                       rc, pid, grantedQoSs[0]);
    return (rc == 1 && count == 1 && grantedQoSs[0] == 0);
}

bool mqtt_client_publish(const char *topic, const uint8_t *payload, uint16_t len_payload)
{
    unsigned char buf[256];
    size_t len = mqtt_controller_build_publish_packet(
        topic, payload, len_payload, buf, sizeof(buf));
    if (len == 0)
    {
        logger_service_log("MQTT Client: build_publish failed");
        return false;
    }
    bool ok = network_controller_tcp_send(buf, len);
    logger_service_log("MQTT Client: PUBLISH '%s' send → %d", topic, ok);
    return ok;
}

void mqtt_client_poll(void)
{
    unsigned char buf[256];
    int n = network_controller_tcp_receive(buf, sizeof(buf));
    if (n <= 0)
        return;

    // peek at packet type
    int msgType = buf[0] >> 4;
    logger_service_log("MQTT Client: packet type %d received, %d bytes", msgType, n);
    if (msgType == PUBLISH)
    {
        logger_service_log("MQTT Client: dispatching PUBLISH to callback");
        unsigned char dup;
        int qos;
        unsigned char retained;
        unsigned short packetid;
        MQTTString topicName;
        unsigned char *payload;
        int payloadlen;

        int rc = MQTTDeserialize_publish(
            &dup,        // OUT: duplicated flag
            &qos,        // OUT: QoS level
            &retained,   // OUT: retained flag
            &packetid,   // OUT: packet identifier (only for QoS>0)
            &topicName,  // OUT: topic name
            &payload,    // OUT: pointer into buf where payload starts
            &payloadlen, // OUT: length of payload
            buf,         // IN: raw buffer
            n            // IN: buffer length
        );
        if (rc == 1 && _msg_cb)
        {
            // topicName.lenstring.data + lenstring.len hold the topic characters
            char tbuf[topicName.lenstring.len + 1];
            memcpy(tbuf, topicName.lenstring.data, topicName.lenstring.len);
            tbuf[topicName.lenstring.len] = '\0';

            _msg_cb(tbuf, payload, payloadlen);
        }
    }
    else if (msgType == MQTT_PINGRESP)
    {
        // ignore
    }
    // you could handle DISCONNECT, etc.
}

void mqtt_client_set_message_callback(mqtt_message_cb_t cb)
{
    _msg_cb = cb;
}

void mqtt_client_disconnect(void)
{
    unsigned char buf[8];
    size_t len = mqtt_controller_build_disconnect_packet(buf, sizeof(buf));
    if (len)
        _send_packet(buf, len);
    network_controller_tcp_close();
}
