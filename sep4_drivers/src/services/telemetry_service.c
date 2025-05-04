// services/telemetry_service.c

#include "services/telemetry_service.h"
#include "controllers/sensor_controller.h"
#include "controllers/network_controller.h"
#include "services/wifi_service.h"
#include "controllers/mqtt_controller.h"               // create_mqtt_*_packet()
#include <string.h>
#include <stdio.h>

/// Broker & client parameters (set in init)
static const char *svc_broker_ip;
static uint16_t    svc_broker_port;
static const char *svc_client_id;
static const char *svc_topic;

/// State flag: have we already connected?
static bool       svc_mqtt_connected = false;

/// Scratch buffers
#define TEL_PKT_BUF_SZ 256
static unsigned char _pkt_buf[TEL_PKT_BUF_SZ];
static char          _json_buf[128];

void telemetry_service_init(const char *broker_ip,
                            uint16_t      broker_port,
                            const char   *client_id,
                            const char   *topic)
{
    svc_broker_ip    = broker_ip;
    svc_broker_port  = broker_port;
    svc_client_id    = client_id;
    svc_topic        = topic;
    svc_mqtt_connected = false;
    mqtt_controller_config_t cfg = {
        .client_id          = svc_client_id,
        .keepalive_interval = 20,
        .cleansession       = 1,
        .MQTTVersion        = 4,
        .username           = NULL,
        .password           = NULL,
    };
    mqtt_controller_init(&cfg);
}

void telemetry_service_poll(void) {
    // Always drain incoming TCP data (e.g. CONNACK or other packets)
    uint8_t tmp[128];
    int n = network_controller_tcp_receive(tmp, sizeof(tmp));
    (void)n;  // we ignore parsing here, but at least flush the buffer
}

bool telemetry_service_publish(void) {
    // 1) Ensure Wi-Fi is up
    if (!wifi_service_is_connected()) {
        return false;
    }

    // 2) If needed, open TCP + send CONNECT
    if (!svc_mqtt_connected) {
        if (!network_controller_tcp_open(svc_broker_ip, svc_broker_port)) {
            return false;
        }
        // build MQTT CONNECT packet
        size_t connlen = create_mqtt_connect_packet(_pkt_buf, TEL_PKT_BUF_SZ);
        if (connlen == 0) {
            return false;
        }
        if (!network_controller_tcp_send(_pkt_buf, connlen)) {
            return false;
        }
        svc_mqtt_connected = true;
        // give broker a moment to reply with CONNACK (it will be
        // drained on the next poll() call)
    }

    // 3) Read latest sensors
    sensor_controller_poll();
    uint8_t hum_i, hum_d, tmp_i, tmp_d;
    hum_i = sensor_controller_get_humidity_integer();
    hum_d = sensor_controller_get_humidity_decimal();
    tmp_i = sensor_controller_get_temperature_integer();
    tmp_d = sensor_controller_get_temperature_decimal();
    uint16_t light = sensor_controller_get_light();

    // 4) Format JSON payload
    //    Example: {"temperature":23.7,"humidity":45.2,"light":502}
    int jlen = snprintf(_json_buf, sizeof(_json_buf),
                        "{\"temperature\":%u.%u,"
                        "\"humidity\":%u.%u,"
                        "\"light\":%u}",
                        tmp_i, tmp_d,
                        hum_i, hum_d,
                        light);
    if (jlen <= 0) {
        return false;
    }

    // 5) Build MQTT PUBLISH packet (QoS0, no retain)
    size_t pktlen = create_mqtt_transmit_packet(
         (char*)svc_topic,
         (unsigned char*)_json_buf,
         _pkt_buf,
         TEL_PKT_BUF_SZ
    );
    if (pktlen == 0) {
        return false;
    }

    // 6) Send it
    if (!network_controller_tcp_send(_pkt_buf, pktlen)) {
        return false;
    }

    return true;
}
