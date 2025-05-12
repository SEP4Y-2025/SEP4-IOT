#include "MQTTPacket.h"
#include <string.h>
#include "wifi.h"
#include "services/logger_service.h"
#include "services/mqtt_service.h"
#include "services/pot_service.h"
#include "config/device_config.h"
#include "config/topics_config.h"

char callback_buff[256];

void mqtt_service_event_callback()
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

    int result = MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName, &payload, &payloadlen, (unsigned char *)callback_buff, 256);

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
      if (strcmp(topic, MQTT_TOPIC_ACTIVATE) == 0)
      {
        logger_service_log("Command received: Activate pot\n");
        pot_service_handle_activate(topic, payload, payloadlen);
      }
      else if (strcmp(topic, MQTT_TOPIC_DEACTIVATE) == 0)
      {
        logger_service_log("Command received: Deactivate pot\n");
        pot_service_handle_deactivate(topic, payload, payloadlen);
      }
      else if (strcmp(topic, MQTT_TOPIC_GET_DATA) == 0)
      {
        logger_service_log("Command received: Get pot data\n");
        pot_service_handle_get_pot_data(topic, payload, payloadlen);
      }
      // TODO: add more handler functions here
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
    // logger_service_log("RECEIVED SUBACK\n");
    break;

  case 13: // MQTT PINGRESP
    // logger_service_log("RECEIVED PINGRESP\n");
    break;

  default: // Not interesting
    sprintf(msg_buf, "Unhandled packet type: %d\n", packet_type);
    logger_service_log(msg_buf);
  }
}

void mqtt_service_send_pingreq(void)
{
  unsigned char buf[10];
  int len = MQTTSerialize_pingreq(buf, sizeof(buf));

  if (len <= 0)
  {
    logger_service_log("Failed to serialize PINGREQ\n");
    return;
  }

  if (wifi_command_TCP_transmit(buf, len) != WIFI_OK)
  {
    logger_service_log("Failed to send PINGREQ\n");
    return;
  }

  logger_service_log("Sent MQTT PINGREQ\n");
}

size_t mqtt_service_create_mqtt_connect_packet(unsigned char *buf, size_t buflen)
{
  logger_service_log("Creating MQTT Connect packet...\n");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  size_t len = 0;

  data.clientID.cstring = DEVICE_ID;
  data.keepAliveInterval = 20;
  data.cleansession = 1;
  data.MQTTVersion = 3;

  len = MQTTSerialize_connect(buf, buflen, &data);
  return len;
}

// Function to create and serialize the MQTT publish packet
WIFI_ERROR_MESSAGE_t mqtt_service_publish(char *topic, unsigned char *payload,
                                          unsigned char *buf, size_t buflen)
{
  MQTTString topicString = MQTTString_initializer;
  size_t payloadlen = strlen((char *)payload);
  size_t len = 0;

  topicString.cstring = topic;
  len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, payload,
                              payloadlen);
  if (len <= 0)
    return WIFI_FAIL;
  logger_service_log("Trying to send to  topic: %s\n", topic);

  // Send the packet over TCP
  logger_service_log("Sending publish packet: %d\n", len);
  return wifi_command_TCP_transmit(buf, len);
}

// Function to create and serialize the MQTT disconnect packet
size_t mqtt_service_create_mqtt_disconnect_packet(unsigned char *buf, size_t buflen)
{
  size_t len = 0;
  len = MQTTSerialize_disconnect(buf, buflen);
  return len;
}

void mqtt_service_poll(void)
{
  // first, only do MQTT if the radio is up
  if (!network_controller_is_ap_connected())
  {
    // nothing to do — Wi-Fi SM will reconnect eventually
    return;
  }

  if (!mqtt_service_is_connected())
  {
    logger_service_log("MQTT: connecting…");
    if (mqtt_service_connect() == MQTT_OK)
    {
      logger_service_log("MQTT: subscribe to topics");
      mqtt_service_subscribe_to_all_topics();
    }
  }
  else
  {
    // already connected → keep-alive ping
    logger_service_log("MQTT: ping");
    mqtt_service_send_pingreq();
  }
}

void mqtt_service_subscribe_to_all_topics(void)
{
  for (int i = 0; i < NUM_TOPICS; i++)
  {
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)mqtt_topic_strings[i];

    WIFI_ERROR_MESSAGE_t result = mqtt_service_subscribe_to_topic(topic);

    if (result != WIFI_OK)
    {
      logger_service_log("Unable to subscribe to topic: %s\n", topic.cstring);
    }
    else
    {
      logger_service_log("Subscribed to topic: %s\n", topic.cstring);
    }
  }
}
WIFI_ERROR_MESSAGE_t mqtt_service_subscribe_to_topic(MQTTString topic)
{
  logger_service_log("Trying to subscribe to  topic: %s\n", topic.cstring);

  uint8_t buffer[128];

  uint16_t packetId = 1; // Can be incremented if you send multiple subscriptions
  int qos = 1;           // QoS level 1 (as expected)

  int len = MQTTSerialize_subscribe(buffer, sizeof(buffer), 0, packetId, 1, &topic, &qos);

  if (len <= 0)
    return WIFI_FAIL;

  packetId++; // Increment packet ID for next subscription

  // Send the subscribe packet over TCP
  logger_service_log("Sending subscribe packet: %d\n", len);
  return wifi_command_TCP_transmit(buffer, len);
}