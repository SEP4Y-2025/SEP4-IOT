// services/telemetry_service.c

#include "services/telemetry_service.h"
#include "controllers/sensor_controller.h"
#include "controllers/network_controller.h"
#include "services/wifi_service.h"
#include "services/mqtt_service.h"
//#include "controllers/mqtt_controller.h"
#include <string.h>
#include <stdio.h>

#define JSON_BUF_SIZE 128
static char _json_buf[JSON_BUF_SIZE];
static const char *svc_topic ="sensor/telemetry";

/// State flag: have we already done the CONNECT?
//static bool       svc_mqtt_connected = false;

/// Scratch buffers
#define TEL_PKT_BUF_SZ 256
static unsigned char _pkt_buf[TEL_PKT_BUF_SZ];
//static char          _json_buf[128];

void telemetry_service_init(void)
{

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

    // return mqtt_service_publish(svc_topic, (uint8_t *)_json_buf, jlen);
    return true;
}

