// services/telemetry_service.c
#include "services/telemetry_service.h"
#include "controllers/sensor_controller.h"
#include "services/mqtt_service.h"
#include <stdio.h>

#define JSON_BUF_SZ   128

static const char *_svc_topic;
static char        _json_buf[JSON_BUF_SZ];

void telemetry_service_init(const char *topic) {
    _svc_topic = topic;
    // make sure sensors are ready
    sensor_controller_init();
}

void telemetry_service_poll(void) {
    // give the MQTT layer a chance to process incoming packets
    mqtt_service_poll();
}

bool telemetry_service_publish(void) {
    // 1) Ensure MQTT session is up (CONNECT + CONNACK under the hood)
    if (!mqtt_service_connect()) {
        return false;
    }

    // 2) Sample sensors
    sensor_controller_poll();
    uint8_t tmp_i = sensor_controller_get_temperature_integer();
    uint8_t tmp_d = sensor_controller_get_temperature_decimal();
    uint8_t hum_i = sensor_controller_get_humidity_integer();
    uint8_t hum_d = sensor_controller_get_humidity_decimal();
    uint16_t light = sensor_controller_get_light();
    uint8_t soil   = sensor_controller_get_soil();

    // 3) Format JSON payload exactly as before
    int jlen = snprintf(_json_buf, JSON_BUF_SZ,
        "{\"temperature\":%u.%u,"
        "\"humidity\":%u.%u,"
        "\"light\":%u,"
        "\"soil\":%u}",
        (unsigned)tmp_i, (unsigned)tmp_d,
        (unsigned)hum_i, (unsigned)hum_d,
        (unsigned)light,
        (unsigned)soil
    );
    if (jlen < 0 || jlen >= JSON_BUF_SZ) {
        return false;
    }

    // 4) Publish via the common MQTT service
    return mqtt_service_publish(_svc_topic,
                                (uint8_t*)_json_buf,
                                (uint16_t)jlen);
}
