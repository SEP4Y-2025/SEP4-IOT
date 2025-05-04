#include "controllers/mqtt_client.h"
#include "controllers/mqtt_controller.h"   // your packet builder
#include "controllers/network_controller.h"
#include "MQTTPacket.h"                    // for parsing
#include <string.h>
#include "mqtt_controller.h"
#include "MQTTProtocol.h"


static mqtt_message_cb_t _msg_cb;
static mqtt_controller_config_t _pkt_cfg;

/** Helper: send a built packet and return success if network send ok */
static bool _send_packet(const unsigned char *pkt, size_t len) {
    return network_controller_tcp_send(pkt, len);
}

void mqtt_client_init(const mqtt_controller_config_t *cfg) {
    memcpy(&_pkt_cfg, cfg, sizeof(_pkt_cfg));
    mqtt_controller_init(cfg);
}

bool mqtt_client_connect(const char *broker_ip, uint16_t broker_port) {
    unsigned char buf[256];
    // 1) TCP open
    if (!network_controller_tcp_open(broker_ip, broker_port)) return false;
    // 2) Build & send CONNECT
    size_t len = mqtt_controller_build_connect_packet(buf, sizeof(buf));
    if (len == 0 || !_send_packet(buf,len)) return false;
    // 3) Wait for CONNACK
    //    (simple blocking read + MQTTDeserialize_connack)
    int n = network_controller_tcp_receive(buf, sizeof(buf));
    if (n <= 0) return false;
    unsigned char connack_rc;
    if (MQTTDeserialize_connack(&connack_rc, &connack_rc, buf, n) != 1
        || connack_rc != 0) {
        return false;
    }
    return true;
}

bool mqtt_client_subscribe(const char *topic) {
    unsigned char buf[256];
    MQTTString tstr = MQTTString_initializer;
    tstr.cstring    = (char*)topic;
    // build SUBSCRIBE packet (packetid=1)
    int len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, 1, 1, &tstr, NULL);
    if (len <= 0 || !_send_packet(buf, len)) return false;
    // wait for SUBACK
    int n = network_controller_tcp_receive(buf, sizeof(buf));
    if (n <= 0) return false;
    unsigned short packetid;
    int          count;
    int          grantedQoSs[1];  // must be at least as large as the maxcount below

    int rc = MQTTDeserialize_suback(
        &packetid,     // OUT: the packet identifier
        1,              // IN:  maxcount (size of your grantedQoSs array)
        &count,        // OUT: how many QoS values were returned
        grantedQoSs,   // OUT: the array of granted QoSs
        buf,           // IN:  the raw buffer you got from TCP
        n              // IN:  its length
    );
    if (rc != 1 || count != 1 || grantedQoSs[0] != 0) {
        // something went wrong (e.g. broker gave a non-zero QoS)
        return false;
    }

    return true;
}

bool mqtt_client_publish(const char *topic,
                         const uint8_t *payload,
                         uint16_t len_payload) {
    unsigned char buf[256];
    size_t len = mqtt_controller_build_publish_packet(
                     topic, payload, len_payload, buf, sizeof(buf));
    if (len == 0) return false;
    return _send_packet(buf, len);
}

void mqtt_client_poll(void) {
    unsigned char buf[256];
    int n = network_controller_tcp_receive(buf, sizeof(buf));
    if (n <= 0) return;

    // peek at packet type
    int msgType = buf[0] >> 4;
    if (msgType == PUBLISH) {
        unsigned char dup;
        int           qos;
        unsigned char retained;
        unsigned short packetid;
        MQTTString    topicName;
        unsigned char *payload;
        int           payloadlen;
    
        int rc = MQTTDeserialize_publish(
            &dup,          // OUT: duplicated flag
            &qos,          // OUT: QoS level
            &retained,     // OUT: retained flag
            &packetid,     // OUT: packet identifier (only for QoS>0)
            &topicName,    // OUT: topic name
            &payload,      // OUT: pointer into buf where payload starts
            &payloadlen,   // OUT: length of payload
            buf,           // IN: raw buffer
            n              // IN: buffer length
        );
        if (rc == 1 && _msg_cb) {
            // topicName.lenstring.data + lenstring.len hold the topic characters
            char tbuf[topicName.lenstring.len + 1];
            memcpy(tbuf, topicName.lenstring.data, topicName.lenstring.len);
            tbuf[topicName.lenstring.len] = '\0';
    
            _msg_cb(tbuf, payload, payloadlen);
        }
    } else if (msgType == MQTT_PINGRESP) {
        // ignore
    }
    // you could handle DISCONNECT, etc.
}

void mqtt_client_set_message_callback(mqtt_message_cb_t cb) {
    _msg_cb = cb;
}

void mqtt_client_disconnect(void) {
    unsigned char buf[8];
    size_t len = mqtt_controller_build_disconnect_packet(buf, sizeof(buf));
    if (len) _send_packet(buf, len);
    network_controller_tcp_close();
}
