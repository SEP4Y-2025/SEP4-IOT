
#include "services/telemetry_service.h"
#include "controllers/sensor_controller.h"
#include "controllers/network_controller.h"
#include "services/wifi_service.h"
#include "services/mqtt_service.h"
#include <string.h>
#include <stdio.h>
#include "config/device_config.h"
#include "config/topics_config.h"
#include "config/watering_config.h"
#include "services/logger_service.h"

#define JSON_BUF_SIZE 128
static char _json_buf[JSON_BUF_SIZE];

/// State flag: have we already done the CONNECT?
// static bool       svc_mqtt_connected = false;

/// Scratch buffers
#define TEL_PKT_BUF_SZ 256
static unsigned char _pkt_buf[TEL_PKT_BUF_SZ];
// static char          _json_buf[128];

void telemetry_service_init(void)
{
}

void telemetry_service_poll(void)
{
    // Drain any incoming bytes (e.g. CONNACK or errors)
    // uint8_t tmp[128];
    //(void)network_controller_tcp_receive(tmp, sizeof(tmp));
}

void telemetry_service_publish(void)
{
    if (is_telemetry_enabled())
    {
        logger_service_log("Attempting to publish telemetry\n");
        sensor_controller_poll();
        uint8_t hum_i = sensor_controller_get_humidity_integer();
        uint8_t hum_d = sensor_controller_get_humidity_decimal();
        uint8_t tmp_i = sensor_controller_get_temperature_integer();
        uint8_t tmp_d = sensor_controller_get_temperature_decimal();
        uint16_t light = sensor_controller_get_light();
        uint8_t soil = sensor_controller_get_soil();

        logger_service_log("Telemetry: %u.%u %u.%u %u %u\n", tmp_i, tmp_d, hum_i, hum_d, light, soil);

        // Format JSON payload without floats
        //    e.g. {"temperature":27.8,"humidity":33.0,"light":502,"soil":123}
        int jlen = snprintf(_json_buf, sizeof(_json_buf),
                            "{\"temperature\":%u.%u,"
                            "\"air_humidity\":%u.%u,"
                            "\"light_intensity\":%u,"
                            "\"soil_humidity\":%u,"
                            "\"plant_pot_id\":\"%s\"}",
                            (unsigned)tmp_i, (unsigned)tmp_d,
                            (unsigned)hum_i, (unsigned)hum_d,
                            (unsigned)light,
                            (unsigned)soil, DEVICE_ID);
        if (jlen <= 0 || jlen >= sizeof(_json_buf))
        {
            return false;
        }

        logger_service_log("New telemetry is build...\n");

        unsigned char buffer[128];

        logger_service_log("Telemetry JSON payload: %s\n", _json_buf);

        WIFI_ERROR_MESSAGE_t result = mqtt_service_publish(MQTT_TOPIC_AUTOMATIC_READINGS, (unsigned char *)_json_buf, buffer, sizeof(buffer));

        if (result != WIFI_OK)
        {
            logger_service_log("Telemetry publish failed\n");
            return false;
        }

        logger_service_log("Telemetry published successfully\n");
        return true;
    }
    else 
    {
        logger_service_log("Telemetry is disabled\n");
        return false;
    }
}
