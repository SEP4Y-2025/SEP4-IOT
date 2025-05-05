// mqtt_service.c
#include "mqtt_service.h"
#include "controllers/network_controller.h"
#include "controllers/mqtt_controller.h"
#include "services/pot_service.h"
#include "services/logger_service.h"
#include "MQTTPacket.h"
#include <string.h>
#include <stdbool.h>

#define MQTT_RX_BUF_SIZE 256
static uint8_t mqtt_rx_buf[MQTT_RX_BUF_SIZE];

// MQTT subscription topics
static const char *TOPIC_ACTIVATE = "/pot_1/activate";
static const char *TOPIC_DEACTIVATE = "/pot_1/deactivate";


// Internal TCP receive handler
static void mqtt_rx_handler(const uint8_t *data, uint16_t len)
{
    logger_service_log("MQTT RX %u bytes", len);
    logger_service_log("Received somethiiiiiiiiing");

    int packet_type = data[0] >> 4;
    logger_service_log("Packet type: %d", packet_type);
    if (packet_type != 3)
        return;

    MQTTString topicName;
    const unsigned char *payload;
    int payloadlen;
    int qos;
    unsigned char dup, retained;
    unsigned short packetid;

    logger_service_log("Attempting to deserialize publish packet...");
    int rc = MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName, &payload, &payloadlen, data, len);
    logger_service_log("Deserialization result: %d", rc);
    if (rc != 1)
    {
        logger_service_log("Failed to parse MQTT PUBLISH");
        return;
    }

    if (strncmp(topicName.lenstring.data, TOPIC_ACTIVATE, topicName.lenstring.len) == 0)
        pot_service_handle_activate(TOPIC_ACTIVATE, payload, payloadlen);
    else if (strncmp(topicName.lenstring.data, TOPIC_DEACTIVATE, topicName.lenstring.len) == 0)
        pot_service_handle_deactivate(TOPIC_DEACTIVATE, payload, payloadlen);
    else
        logger_service_log("Unhandled topic: %.*s", topicName.lenstring.len, topicName.lenstring.data);
}

void mqtt_service_init(void)
{
    network_controller_set_tcp_callback(mqtt_rx_handler);
    logger_service_log("RX setup done");

    // unsigned char connect_pkt[128];
    // int connect_len = mqtt_controller_build_connect_packet(connect_pkt, sizeof(connect_pkt));
    // network_controller_tcp_send(connect_pkt, connect_len);
}

void mqtt_subscribe()
{
    // Build SUBSCRIBE packet
    MQTTString topicFilter1 = MQTTString_initializer;
    topicFilter1.cstring = (char *)TOPIC_ACTIVATE;

    MQTTString topicFilter2 = MQTTString_initializer;
    topicFilter2.cstring = (char *)TOPIC_DEACTIVATE;

    unsigned char pkt[128];
    int qos1[] = {0};

    int pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt),
                                         0,             // dup
                                         1,             // packet ID
                                         1,             // count
                                         &topicFilter1, // topicFilters[]
                                         qos1           // requestedQoSs[]
    );
    network_controller_tcp_send(pkt, pktlen);

    int qos2[] = {0};

    pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt),
                                     0,             // dup
                                     2,             // packet ID (should be unique per SUBSCRIBE)
                                     1,             // count
                                     &topicFilter2, // topicFilters[]
                                     qos2           // requestedQoSs[]
    );
    network_controller_tcp_send(pkt, pktlen);
    logger_service_log("SUBSCRIBED");

    // int pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt), 0, 1,
    //                                      &topicFilter1, (int[]){0});
    // network_controller_tcp_send(pkt, pktlen);

    // pktlen = MQTTSerialize_subscribe(pkt, sizeof(pkt), 1, 1,
    //                                  &topicFilter2, (int[]){0});
    // network_controller_tcp_send(pkt, pktlen);
}


void mqtt_service_poll(void)
{
    //
}
