
#include "services/telemetry_service.h"
//#include "controllers/sensor_controller.h"
#include "services/sensor_service.h"
#include "controllers/network_controller.h"
#include "services/wifi_service.h"
#include "services/mqtt_service.h"
#include <string.h>
#include <stdio.h>
#include "config/device_config.h"
#include "config/topics_config.h"
#include "state/watering_state.h"
#include "services/logger_service.h"
#include "utils/adc_to_percentage_converter.h"
#include "utils/adc_to_lux_converter.h"

#define JSON_BUF_SIZE 128
static char _json_buf[JSON_BUF_SIZE];

void telemetry_service_init(void)
{
}

void telemetry_service_publish(void)
{
    if (is_telemetry_enabled())
    {
        LOG("Attempting to publish telemetry\n");

        sensor_service_read();
        uint8_t hum_i = sensor_service_get_humidity_integer();
        uint8_t hum_d = sensor_service_get_humidity_decimal();
        uint8_t tmp_i = sensor_service_get_temperature_integer();
        uint8_t tmp_d = sensor_service_get_temperature_decimal();
        uint16_t light = sensor_service_get_light();
        uint8_t soil = sensor_service_get_soil();

        LOG("Telemetry: %u.%u %u.%u %u %u\n", tmp_i, tmp_d, hum_i, hum_d, light, soil);

        uint32_t light_lux = convert_adc_to_lux(light);
        uint8_t soil_percentage = convert_adc_to_percentage(soil);

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
                            (unsigned)light_lux,
                            (unsigned)soil_percentage, DEVICE_ID);
        if (jlen <= 0 || jlen >= sizeof(_json_buf))
        {
            return;
        }

        LOG("New telemetry is build...\n");

        LOG("Telemetry JSON payload: %s\n", _json_buf);

        if (mqtt_service_publish(
                MQTT_TOPIC_AUTOMATIC_READINGS,
                _json_buf) != MQTT_OK)
        {
            LOG("Telemetry publish failed\n");
            return;
        }

        LOG("Telemetry published successfully\n");
        return;
    }
    else
    {
        LOG("Telemetry is disabled\n");
        return;
    }
}
