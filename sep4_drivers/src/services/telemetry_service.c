// services/telemetry_service.c

#include "services/telemetry_service.h"
#include "controllers/sensor_controller.h"
#include "controllers/network_controller.h"
#include "services/wifi_service.h"
#include "controllers/mqtt_controller.h"
#include <string.h>
#include <stdio.h>

/// Broker & client parameters (set in init)
static const char *svc_broker_ip;
static uint16_t    svc_broker_port;
static const char *svc_client_id;
static const char *svc_topic;

/// State flag: have we already done the CONNECT?
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

    // Configure the MQTT controller once
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
    // Drain any incoming bytes (e.g. CONNACK or errors)
    //uint8_t tmp[128];
    //(void)network_controller_tcp_receive(tmp, sizeof(tmp));
}

bool telemetry_service_publish(void) {
    // 1) Ensure Wi-Fi is up
    if (!wifi_service_is_connected()) {
        return false;
    }

    // 2) If needed, open TCP + send MQTT CONNECT
    if (!svc_mqtt_connected) {
        if (!network_controller_tcp_open(svc_broker_ip, svc_broker_port)) {
            return false;
        }
        size_t connlen = mqtt_controller_build_connect_packet(_pkt_buf, TEL_PKT_BUF_SZ);
        if (connlen == 0) {
            return false;
        }
        if (!network_controller_tcp_send(_pkt_buf, connlen)) {
            return false;
        }
        svc_mqtt_connected = true;
        // CONNACK will be drained in the next poll()
    }

    // 3) Read latest sensors
    sensor_controller_poll();
    uint8_t hum_i = sensor_controller_get_humidity_integer();
    uint8_t hum_d = sensor_controller_get_humidity_decimal();
    uint8_t tmp_i = sensor_controller_get_temperature_integer();
    uint8_t tmp_d = sensor_controller_get_temperature_decimal();
    uint16_t light = sensor_controller_get_light();
    uint8_t soil   = sensor_controller_get_soil();

    // 4) Format JSON payload without floats
    //    e.g. {"temperature":27.8,"humidity":33.0,"light":502,"soil":123}
    int jlen = snprintf(_json_buf, sizeof(_json_buf),
        "{\"temperature\":%u.%u,"
        "\"humidity\":%u.%u,"
        "\"light\":%u,"
        "\"soil\":%u}",
        (unsigned)tmp_i, (unsigned)tmp_d,
        (unsigned)hum_i, (unsigned)hum_d,
        (unsigned)light,
        (unsigned)soil
    );
    if (jlen <= 0) {
        return false;
    }

    // 5) Build MQTT PUBLISH packet (QoS0, no retain)
    size_t pktlen = mqtt_controller_build_publish_packet(
        svc_topic,
        (unsigned char*)_json_buf,
        (size_t)jlen,
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

