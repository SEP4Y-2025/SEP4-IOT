#include "services/pot_service.h"
#include "services/logger_service.h"
#include "services/telemetry_service.h"
#include "services/mqtt_service.h"
#include "controllers/network_controller.h"
#include "services/sensor_service.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "config/device_config.h"
#include "config/topics_config.h"
#include "state/watering_state.h"
#include "utils/json_parser.h"
#include "utils/adc_to_percentage_converter.h"
#include "utils/adc_to_lux_converter.h"
#include "includes.h"

#define JSON_BUF_SIZE 512
static char _json_buf[JSON_BUF_SIZE];

void pot_service_handle_activate(const char *topic, const uint8_t *payload, uint16_t len)
{
    set_telemetry_enabled(true);

    LOG("Pot ACTIVATED");

    uint32_t frequency = 0, dosage = 0;
    if (parse_watering_payload((const char *)payload, &frequency, &dosage))
    {
        update_watering_settings(frequency, dosage);
        LOG("Watering settings updated and saved to EEPROM");
        set_watering_enabled(true);
        log_stored_watering_state();
    }
    else
    {
        LOG("Failed to parse watering payload");
    }

    // Send ACK
    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_error_t res = mqtt_service_publish(response_topic, "{\"status\":\"ok\"}");
    if (res != MQTT_OK)
    {
        LOG("Pot activate ACK publish failed\n");
    }
}

void pot_service_handle_deactivate(const char *topic, const uint8_t *payload, uint16_t len)
{
    set_telemetry_enabled(false);
    set_watering_enabled(false);

    LOG("Pot DEACTIVATED");

    // Send ACK
    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_error_t res = mqtt_service_publish(response_topic, "{\"status\":\"ok\"}");
    if (res != MQTT_OK)
    {
        LOG("Pot deactivate ACK publish failed\n");
    }
}

bool pot_service_handle_get_pot_data(const char *topic, const uint8_t *payload, uint16_t len)
{
    LOG("Pot data requested");

    // sensor_controller_read();
    sensor_service_read();
    uint8_t hum_i = sensor_service_get_humidity_integer();
    uint8_t hum_d = sensor_service_get_humidity_decimal();
    uint8_t tmp_i = sensor_service_get_temperature_integer();
    uint8_t tmp_d = sensor_service_get_temperature_decimal();
    uint16_t light = sensor_service_get_light();
    uint8_t soil = sensor_service_get_soil();
    uint8_t water_level = sensor_service_get_water_level_percentage();

    LOG("Pot data!!!!: %u.%u %u.%u %u %u\n", tmp_i, tmp_d, hum_i, hum_d, light, soil);

    uint32_t light_lux = convert_adc_to_lux(light);
    uint8_t soil_percentage = convert_adc_to_percentage(soil);

    // Format JSON payload without floats
    //    e.g. {"temperature":27.8,"humidity":33.0,"light":502,"soil":123}
    int jlen = snprintf(_json_buf, sizeof(_json_buf),
                        "{\"temperature_celsius\":%u.%u,"
                        "\"air_humidity_percentage\":%u.%u,"
                        "\"light_intensity_lux\":%u,"
                        "\"soil_humidity_percentage\":%u,"
                        "\"plant_pot_id\":\"%s\","
                        "\"water_tank_capacity_ml\":%u,"
                        "\"water_level_percentage\":%u,"
                        "\"status\":\"ok\"}",
                        (unsigned)tmp_i, (unsigned)tmp_d,
                        (unsigned)hum_i, (unsigned)hum_d,
                        (unsigned)light_lux,
                        (unsigned)soil_percentage, DEVICE_ID, WATER_TANK_CAPACITY_ML, water_level);

    LOG("Pot data is built...\n");

    if (jlen <= 0 || jlen >= sizeof(_json_buf))
    {
        return false;
    }
    LOG("The length is ok...\n");

    unsigned char buffer[JSON_BUF_SIZE];

    LOG("Pot JSON payload: %s\n", _json_buf);

    // Publish it
    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_error_t res = mqtt_service_publish(response_topic, _json_buf);
    if (res != MQTT_OK)
    {
        LOG("Pot data publish failed\n");
        return false;
    }

    LOG("Pot data published successfully\n");
    return true;
}