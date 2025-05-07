#include "scheduler.h"
#include "services/wifi_service.h"
#include "services/telemetry_service.h"
#include "services/sensor_service.h"
#include "services/initializer.h"
#include "services/logger_service.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include "MQTTPacket.h"
#include <util/delay.h>
#include <string.h>

#include "services/mqtt_service.h"
#include "services/pot_service.h"

#include <avr/interrupt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
//  … other includes …

static char callback_buff[256];

void mqtt_event_cb()
{
    // Check if we have valid data
    if (callback_buff[0] == 0)
    {
        logger_service_log("Empty buffer received\n");
        return;
    }

    // Extract MQTT packet type from first byte
    unsigned char packet_type = (callback_buff[0] >> 4) & 0x0F;

    // Print packet type
    char msg_buf[250];
    sprintf(msg_buf, "MQTT packet received - Type: %d\n", packet_type);
    logger_service_log(msg_buf);

    switch (packet_type)
    {
    case 2: // MQTT CONNACK
        logger_service_log("RECEIVED CONNACK\n");
        break;

    case 3: // MQTT PUBLISH
    {
        unsigned char dup = 0;
        int qos = 0;
        unsigned char retained = 0;
        unsigned short packetid = 0;
        MQTTString topicName = {0};
        unsigned char *payload = NULL;
        int payloadlen = 0;

        int result = MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName, &payload, &payloadlen, (unsigned char*)callback_buff, 256);

        if (result == 1)
        {
            // create topic string
            char topic[64] = {0};
            if (topicName.lenstring.len < sizeof(topic))
            {
                memcpy(topic, topicName.lenstring.data, topicName.lenstring.len);
                topic[topicName.lenstring.len] = '\0';
            }

            // Create payload string
            char payload_str[128] = {0};
            if (payloadlen < sizeof(payload_str))
            {
                memcpy(payload_str, payload, payloadlen);
                payload_str[payloadlen] = '\0';
            }

            sprintf(msg_buf, "PUBLISH: Topic='%s', Payload='%s', QoS=%d\n",
                    topic, payload_str, qos);
            logger_service_log(msg_buf);

            // Check for topic
            if (strcmp(topic, "/pot_1/activate") == 0)
            {
                logger_service_log("Command received: Activate pot\n");
            }
            else if (strcmp(topic, "/pot_1/deactivate") == 0)
            {
                logger_service_log("Command received: Deactivate pot\n");
            }
            else
            {
                logger_service_log("Unknown topic received\n");
            }
        }
        else
        {
            logger_service_log("Failed to parse PUBLISH packet\n");
        }
        break;
    }

    case 9: // MQTT SUBACK
        logger_service_log("RECEIVED SUBACK\n");
        break;

    default:
        sprintf(msg_buf, "Unhandled packet type: %d\n", packet_type);
        logger_service_log(msg_buf);
    }
}

int main(void)
{
    initialize_system();

    logger_service_log("Initialization complete.\n");

    if (setup_network_connection("Betelgeuse", "Hello World", "192.168.120.58", 1883,
                                 mqtt_event_cb, callback_buff) != WIFI_OK)
    {
        logger_service_log("Error setting up network connection!\n");
        return -1;
    }

    logger_service_log("Connected to WiFi and MQTT broker!\n");

    _delay_ms(5000);

    MQTTString topic = MQTTString_initializer;
    topic.cstring = "/pot_1/activate";

    WIFI_ERROR_MESSAGE_t subscribe_message = mqtt_subscribe_to_topic(topic);
    if (subscribe_message != WIFI_OK)
    {
        logger_service_log("Unable to send subscribe packet!\n");
    }
    else
    {
        logger_service_log("Sent subscribe packet!\n");
    }

    MQTTString topic2 = MQTTString_initializer;
    topic2.cstring = "/pot_1/deactivate";

    WIFI_ERROR_MESSAGE_t subscribe_message2 = mqtt_subscribe_to_topic(topic2);
    if (subscribe_message2 != WIFI_OK)
    {
        logger_service_log("Unable to send subscribe packet!\n");
    }
    else
    {
        logger_service_log("Sent subscribe packet!\n");
    }

    //periodic_task_init_a(loop, 2000);

    while (1)
    {
    }

    return 0;
}
