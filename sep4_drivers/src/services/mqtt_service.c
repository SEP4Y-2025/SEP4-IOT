#include "services/mqtt_service.h"
#include "controllers/mqtt_controller.h"
#include "controllers/network_controller.h"
#include "services/logger_service.h"
#include "services/pot_service.h"
#include "services/wifi_service.h"
#include "MQTTPacket.h"
#include <string.h>
#include <stdbool.h>

#define MAX_SUBSCRIPTIONS 5
#define MQTT_RX_BUF_SIZE 256
#define MQTT_PKT_BUF_SIZE 256

static struct
{
    const char *topic;
    mqtt_message_handler_t handler;
} subscriptions[MAX_SUBSCRIPTIONS];

static uint8_t mqtt_rx_buf[MQTT_RX_BUF_SIZE];
static unsigned char pkt_buf[MQTT_PKT_BUF_SIZE];

static const char *svc_broker_ip;
static uint16_t svc_broker_port;
static const char *svc_client_id;

static bool svc_mqtt_connected = false; // TCP connected and CONNECT sent
static bool svc_mqtt_ready = false;     // CONNACK received, ready to publish/subscribe

static void on_mqtt_connack(void)
{
    svc_mqtt_ready = true;
    logger_service_log("MQTT is now ready (CONNACK received)");

    mqtt_service_subscribe("/pot_1/activate", pot_service_handle_activate);
    mqtt_service_subscribe("/pot_1/deactivate", pot_service_handle_deactivate);
    logger_service_log("Subscriptions sent");
}

static void mqtt_rx_handler(const uint8_t *data, uint16_t len)
{
    int packet_type = data[0] >> 4;
    if (packet_type == CONNACK)
    {
        mqtt_controller_handle_connack(data, len);
        return;
    }

    if (packet_type != 3)
        return; // Not a PUBLISH

    MQTTString topicName;
    const unsigned char *payload;
    int payloadlen;
    int qos;
    unsigned char dup, retained;
    unsigned short packetid;

    int rc = MQTTDeserialize_publish(&dup, &qos, &retained, &packetid,
                                     &topicName, &payload, &payloadlen, data, len);
    if (rc != 1)
        return;

    char topic_buf[128];
    int topic_len = topicName.lenstring.len;

    // strncpy(topic_buf, topicName.lenstring.data, topic_len);

    if (topic_len >= sizeof(topic_buf))
        topic_len = sizeof(topic_buf) - 1;
    memcpy(topic_buf, topicName.lenstring.data, topic_len);
    topic_buf[topic_len] = '\0';

    for (int i = 0; i < MAX_SUBSCRIPTIONS; ++i)
    {
        if (subscriptions[i].topic &&
            strcmp(subscriptions[i].topic, topic_buf) == 0)
        {
            subscriptions[i].handler(topic_buf, payload, payloadlen);
            return;
        }
    }
}

void mqtt_service_init(const char *broker_ip,
                       uint16_t broker_port,
                       const char *client_id)
{
    svc_broker_ip = broker_ip;
    svc_broker_port = broker_port;
    svc_client_id = client_id;
    svc_mqtt_connected = false;

    network_controller_set_tcp_callback(mqtt_rx_handler);

    mqtt_controller_config_t cfg = {
        .client_id = svc_client_id,
        .keepalive_interval = 20,
        .cleansession = 1,
        .MQTTVersion = 4,
        .username = NULL,
        .password = NULL};

    mqtt_controller_set_connack_callback(on_mqtt_connack);

    mqtt_controller_init(&cfg);
}

bool mqtt_service_subscribe(const char *topic, mqtt_message_handler_t handler)
{
    if (!svc_mqtt_ready)
    {
        logger_service_log("Cannot subscribe yet: MQTT not ready");
        return false;
    }

    for (int i = 0; i < MAX_SUBSCRIPTIONS; ++i)
    {
        if (subscriptions[i].topic == NULL)
        {
            subscriptions[i].topic = topic;
            subscriptions[i].handler = handler;

            MQTTString mqtt_topic = MQTTString_initializer;
            mqtt_topic.cstring = (char *)topic;

            int qos[] = {0};
            int pktlen = MQTTSerialize_subscribe(pkt_buf, sizeof(pkt_buf),
                                                 0, i + 1, 1, &mqtt_topic, qos);

            if (!network_controller_tcp_send(pkt_buf, pktlen))
            {
                svc_mqtt_connected = false;
                svc_mqtt_ready = false;
                logger_service_log("MQTT PING send failed — resetting connection");
                return false;
            }

            network_controller_tcp_send(pkt_buf, pktlen);
            return true;
        }
    }
    return false;
}

bool mqtt_service_publish(const char *topic,
                          const uint8_t *payload,
                          uint16_t len)
{
    if (!wifi_service_is_connected())
    {
        logger_service_log("Failed to publish telemetry because wifi");
        return false;
    }

    if (!svc_mqtt_ready)
    {
        logger_service_log("Failed to publish telemetry because MQTT not ready");
        return false; // Still waiting for CONNACK
    }

    size_t pktlen = mqtt_controller_build_publish_packet(
        topic, payload, len, pkt_buf, sizeof(pkt_buf));

    if (pktlen == 0)
        return false;

    if (!network_controller_tcp_send(pkt_buf, pktlen))
    {
        svc_mqtt_connected = false;
        svc_mqtt_ready = false;
        logger_service_log("MQTT PUBLISH send failed — resetting connection");
        return false;
    }
    return true;
}

void mqtt_service_poll(void)
{
    static uint32_t last_ping = 0;

    // Check if TCP connection is still alive
    if (svc_mqtt_connected && !wifi_service_is_connected())
    {
        svc_mqtt_connected = false;
        svc_mqtt_ready = false;
        logger_service_log("MQTT connection lost (TCP dead)");
        //return;
    }
    if (!svc_mqtt_connected)
    {
        if (network_controller_tcp_open(svc_broker_ip, svc_broker_port))
        {
            size_t connlen = mqtt_controller_build_connect_packet(pkt_buf, sizeof(pkt_buf));
            if (connlen && network_controller_tcp_send(pkt_buf, connlen))
            {
                svc_mqtt_connected = true;
                svc_mqtt_ready = false;
                logger_service_log("MQTT CONNECT sent");
            }
        }
        return;
    }

    if (svc_mqtt_ready && scheduler_elapsed(&last_ping, 15000))
    {
        size_t pktlen = mqtt_controller_build_ping_packet(pkt_buf, sizeof(pkt_buf));
        if (pktlen && !network_controller_tcp_send(pkt_buf, pktlen))
        {
            svc_mqtt_connected = false;
            svc_mqtt_ready = false;
            logger_service_log("MQTT PING send failed — resetting connection");
            return;
        }
        else
            logger_service_log("MQTT PING sent");
    }
    scheduler_mark(&last_ping);
}

// // mqtt_service.c
// #include "mqtt_service.h"
// #include "controllers/network_controller.h"
// #include "controllers/mqtt_controller.h"
// #include "services/pot_service.h"
// #include "services/logger_service.h"
// #include "MQTTPacket.h"
// #include <string.h>
// #include <stdbool.h>
// #include "services/wifi_service.h"

// #define MQTT_RX_BUF_SIZE 256
// static uint8_t mqtt_rx_buf[MQTT_RX_BUF_SIZE];

// // MQTT subscription topics
// static const char *TOPIC_ACTIVATE = "/pot_1/activate";
// static const char *TOPIC_DEACTIVATE = "/pot_1/deactivate";

// // Internal TCP receive handler
// static void mqtt_rx_handler(const uint8_t *data, uint16_t len)
// {
//     logger_service_log("MQTT RX %u bytes", len);
//     logger_service_log("Received somethiiiiiiiiing");

//     int packet_type = data[0] >> 4;

//     if (packet_type == CONNACK) {
//         mqtt_controller_handle_connack(data, len);
//     }
//     logger_service_log("Packet type: %d", packet_type);
//     if (packet_type != 3)
//         return;

//     MQTTString topicName;
//     const unsigned char *payload;
//     int payloadlen;
//     int qos;
//     unsigned char dup, retained;
//     unsigned short packetid;

//     logger_service_log("Attempting to deserialize publish packet...");
//     int rc = MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName, &payload, &payloadlen, data, len);
//     logger_service_log("Deserialization result: %d", rc);
//     if (rc != 1)
//     {
//         logger_service_log("Failed to parse MQTT PUBLISH");
//         return;
//     }

//     if (strncmp(topicName.lenstring.data, TOPIC_ACTIVATE, topicName.lenstring.len) == 0)
//     {
//         logger_service_log("Call handle activate...");
//         pot_service_handle_activate(TOPIC_ACTIVATE, payload, payloadlen);
//     }
//     else if (strncmp(topicName.lenstring.data, TOPIC_DEACTIVATE, topicName.lenstring.len) == 0)
//     {
//         logger_service_log("Call handle deactivate...");
//         pot_service_handle_deactivate(TOPIC_DEACTIVATE, payload, payloadlen);
//     }
//     else
//         logger_service_log("Unhandled topic: %.*s", topicName.lenstring.len, topicName.lenstring.data);
// }
// /// Broker & client parameters (set in init)
// static const char *svc_broker_ip;
// static uint16_t svc_broker_port;
// static const char *svc_client_id;
// static const char *svc_topic;

// /// State flag: have we already done the CONNECT?
// static bool svc_mqtt_connected = false;

// /// Scratch buffers
// #define TEL_PKT_BUF_SZ 256
// static unsigned char _pkt_buf[TEL_PKT_BUF_SZ];
// static char _json_buf[128];

// void mqtt_service_init(const char *broker_ip,
//                        uint16_t broker_port,
//                        const char *client_id,
//                        const char *topic)
// {
//     svc_broker_ip = broker_ip;
//     svc_broker_port = broker_port;
//     svc_client_id = client_id;
//     svc_topic = topic;
//     svc_mqtt_connected = false;

//     network_controller_set_tcp_callback(mqtt_rx_handler);
//     logger_service_log("RX setup done");

//     // Configure the MQTT controller once
//     mqtt_controller_config_t cfg = {
//         .client_id = svc_client_id,
//         .keepalive_interval = 20,
//         .cleansession = 1,
//         .MQTTVersion = 4,
//         .username = NULL,
//         .password = NULL,
//     };
//     mqtt_controller_init(&cfg);
// }
// // void mqtt_service_init(void)
// // {
// //     network_controller_set_tcp_callback(mqtt_rx_handler);
// //     logger_service_log("RX setup done");

// //     // unsigned char connect_pkt[128];
// //     // int connect_len = mqtt_controller_build_connect_packet(connect_pkt, sizeof(connect_pkt));
// //     // network_controller_tcp_send(connect_pkt, connect_len);
// // }

// void mqtt_subscribe()
// {
//     // Build SUBSCRIBE packet
//     MQTTString topicFilter1 = MQTTString_initializer;
//     topicFilter1.cstring = (char *)TOPIC_ACTIVATE;

//     MQTTString topicFilter2 = MQTTString_initializer;
//     topicFilter2.cstring = (char *)TOPIC_DEACTIVATE;

//     unsigned char pkt[128];
//     int qos1[] = {0};

//     int pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt),
//                                          0,             // dup
//                                          1,             // packet ID
//                                          1,             // count
//                                          &topicFilter1, // topicFilters[]
//                                          qos1           // requestedQoSs[]
//     );
//     network_controller_tcp_send(pkt, pktlen);

//     int qos2[] = {0};

//     pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt),
//                                      0,             // dup
//                                      2,             // packet ID (should be unique per SUBSCRIBE)
//                                      1,             // count
//                                      &topicFilter2, // topicFilters[]
//                                      qos2           // requestedQoSs[]
//     );
//     network_controller_tcp_send(pkt, pktlen);
//     logger_service_log("SUBSCRIBED");

//     // int pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt), 0, 1,
//     //                                      &topicFilter1, (int[]){0});
//     // network_controller_tcp_send(pkt, pktlen);

//     // pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt), 1, 1,
//     //                                  &topicFilter2, (int[]){0});
//     // network_controller_tcp_send(pkt, pktlen);
// }

// void mqtt_service_poll(void)
// {
//     static uint32_t last_ping = 0;
//     if (scheduler_elapsed(&last_ping, 15000))
//     {
//         size_t pktlen = mqtt_controller_build_ping_packet(_pkt_buf, TEL_PKT_BUF_SZ);
//         if (pktlen > 0)
//         {
//             network_controller_tcp_send(_pkt_buf, pktlen);
//         }
//         scheduler_mark(&last_ping);
//     }
// }
