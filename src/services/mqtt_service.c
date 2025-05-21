#include "MQTTPacket.h"
#include <string.h>
#include "wifi.h"
#include "services/logger_service.h"
#include "services/mqtt_service.h"
#include "services/pot_service.h"
#include "services/watering_service.h"
#include "config/device_config.h"
#include "config/topics_config.h"
#include "controllers/network_controller.h"
#include "state/watering_state.h"
// #include "cont"
char callback_buff[256];

static volatile bool _mqtt_up = false;
static uint32_t _poll_ms;

static char *s_broker_ip;
static uint16_t s_broker_port;

void mqtt_service_init(char *broker_ip, uint16_t broker_port)
{
  s_broker_ip = broker_ip;
  s_broker_port = broker_port;
  LOG("MQTT service initialized (%s:%u)\n", broker_ip, broker_port);
}

void mqtt_service_event_callback()
{
  // Check if we have valid data
  if (callback_buff[0] == 0)
  {
    LOG("Empty buffer received\n");
    return;
  }

  // Extract MQTT packet type from first byte
  unsigned char packet_type = (callback_buff[0] >> 4) & 0x0F;

  // Print packet type
  char msg_buf[250];

  switch (packet_type)
  {
  case 2: // MQTT CONNACK
    LOG("RECEIVED CONNACK\n");
    _mqtt_up = true;
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
      LOG(msg_buf);

      // Check for topic
      if (strcmp(topic, MQTT_TOPIC_ACTIVATE) == 0)
      {
        LOG("Command received: Activate pot\n");
        pot_service_handle_activate(topic, payload, payloadlen);
      }
      else if (strcmp(topic, MQTT_TOPIC_DEACTIVATE) == 0)
      {
        LOG("Command received: Deactivate pot\n");
        pot_service_handle_deactivate(topic, payload, payloadlen);
      }
      else if (strcmp(topic, MQTT_TOPIC_GET_DATA) == 0)
      {
        LOG("Command received: Get pot data\n");
        pot_service_handle_get_pot_data(topic, payload, payloadlen);
      }
      else if (strcmp(topic, MQTT_TOPIC_WATERING) == 0)
      {
        LOG("Command received: Watering\n");
        watering_service_handle_settings_update(topic, payload, payloadlen);
        LOG("Watering settings updated: Frequency=%lu, Dosage=%lu\n", get_watering_frequency(), get_water_dosage());
      }

      // TODO: add more handler functions here
      else
      {
        LOG("Unknown topic received\n");
      }
    }
    else
    {
      LOG("Failed to parse PUBLISH packet\n");
    }
    break;
  }

  case 9: // MQTT SUBACK
    // LOG("RECEIVED SUBACK\n");
    break;

  case 13: // MQTT PINGRESP
    // LOG("RECEIVED PINGRESP\n");
    break;

  default: // Not interesting
    sprintf(msg_buf, "Unhandled packet type: %d\n", packet_type);
    LOG(msg_buf);
  }
}

bool mqtt_service_is_connected(void) { return _mqtt_up; }

mqtt_error_t mqtt_service_connect(void)
{
  // 1) Ensure Wi-Fi AP is up
  if (!network_controller_is_ap_connected())
    return MQTT_FAIL;

  // 2) Ensure TCP is open
  if (!network_controller_is_tcp_connected())
  {
    if (network_controller_tcp_open(s_broker_ip, s_broker_port) != WIFI_OK)
    {
      LOG("MQTT: TCP open failed\n");
      return MQTT_FAIL;
    }
  }

  // 3) Build and send MQTT CONNECT packet
  unsigned char buf[200];
  int len = mqtt_service_create_mqtt_connect_packet(buf, sizeof(buf));
  if (len <= 0)
  {
    LOG("MQTT: serialize CONNECT failed\n");
    return MQTT_FAIL;
  }

  if (wifi_command_TCP_transmit(buf, len) != WIFI_OK)
  {
    LOG("MQTT: send CONNECT failed, attempting TCP reconnect...\n");
    network_controller_tcp_close();
    if (network_controller_tcp_open(s_broker_ip, s_broker_port) != WIFI_OK)
    {
      LOG("MQTT: TCP reconnect failed\n");
      return MQTT_FAIL;
    }

    if (wifi_command_TCP_transmit(buf, len) != WIFI_OK)
    {
      LOG("MQTT: send CONNECT after reconnect failed\n");
      return MQTT_FAIL;
    }
  }

  // 4) Wait (blocking) up to 5 s for CONNACK
  uint32_t start = scheduler_millis();
  while (scheduler_millis() - start < 5000)
  {
    if (_mqtt_up)
      return MQTT_OK;
  }
  LOG("MQTT: CONNACK timeout\n");
  return MQTT_FAIL;
}

size_t mqtt_service_create_mqtt_connect_packet(unsigned char *buf, size_t buflen)
{
  LOG("Creating MQTT Connect packet...\n");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  size_t len = 0;

  data.clientID.cstring = DEVICE_ID;
  data.keepAliveInterval = 20;
  data.cleansession = 1;
  data.MQTTVersion = 3;

  len = MQTTSerialize_connect(buf, buflen, &data);
  return len;
}

size_t mqtt_service_create_mqtt_disconnect_packet(unsigned char *buf, size_t buflen)
{
  size_t len = 0;
  len = MQTTSerialize_disconnect(buf, buflen);
  return len;
}

mqtt_error_t mqtt_service_subscribe_to_all_topics(void)
{
  if (!_mqtt_up)
    return MQTT_FAIL;

  for (int i = 0; i < NUM_TOPICS; i++)
  {
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)mqtt_topic_strings[i];
    if (mqtt_service_subscribe_to_topic(topic) != WIFI_OK)
    {
      LOG("MQTT: subscribe failed %s\n", topic.cstring);
      return MQTT_FAIL;
    }
    LOG("MQTT: subscribed %s\n", topic.cstring);
  }
  return MQTT_OK;
}

WIFI_ERROR_MESSAGE_t mqtt_service_subscribe_to_topic(MQTTString topic)
{
  LOG("Trying to subscribe to  topic: %s\n", topic.cstring);

  uint8_t buffer[128];

  uint16_t packetId = 1;
  int qos = 1; // QoS level 1 (as expected)

  int len = MQTTSerialize_subscribe(buffer, sizeof(buffer), 0, packetId, 1, &topic, &qos);

  if (len <= 0)
    return WIFI_FAIL;

  packetId++; // Increment packet ID for next subscription

  LOG("Sending subscribe packet: %d\n", len);
  return wifi_command_TCP_transmit(buffer, len);
}

mqtt_error_t mqtt_service_send_pingreq(void)
{
  if (!_mqtt_up)
    return MQTT_FAIL;
  unsigned char buf[2];
  int len = MQTTSerialize_pingreq(buf, sizeof(buf));
  if (len <= 0 || wifi_command_TCP_transmit(buf, len) != WIFI_OK)
  {
    _mqtt_up = false;
    LOG("MQTT: PINGREQ failed\n");
    return MQTT_FAIL;
  }
  return MQTT_OK;
}

mqtt_error_t mqtt_service_publish(const char *topic, const char *payload)
{
  if (!_mqtt_up)
    return MQTT_FAIL;

  unsigned char buf[256];
  MQTTString tstr = MQTTString_initializer;
  tstr.cstring = (char *)topic;
  int payloadlen = (int)strlen(payload);
  int len = MQTTSerialize_publish(
      buf, sizeof(buf),
      0, 0, 0, 0,
      tstr,
      (unsigned char *)payload,
      payloadlen);
  if (len <= 0)
  {
    LOG("MQTT: serialize PUBLISH failed\n");
    return MQTT_FAIL;
  }
  if (wifi_command_TCP_transmit(buf, len) != WIFI_OK)
  {
    _mqtt_up = false;
    LOG("MQTT: send PUBLISH failed\n");
    return MQTT_FAIL;
  }
  return MQTT_OK;
}

void mqtt_service_poll(void)
{
  LOG("MQTT: pinging");
  // If Wi-Fi AP drops, mark MQTT down so we'll reconnect later
  if (!network_controller_is_ap_connected())
  {
    _mqtt_up = false;
    return;
  }

  if (!_mqtt_up)
  {
    LOG("MQTT: connecting…\n");
    if (mqtt_service_connect() == MQTT_OK)
      mqtt_service_subscribe_to_all_topics();
  }
  else
  {
    mqtt_service_send_pingreq();
  }
}
