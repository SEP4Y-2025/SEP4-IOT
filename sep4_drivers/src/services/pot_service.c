#include "services/pot_service.h"
#include "services/logger_service.h"
#include "services/telemetry_service.h"
// #include "controllers/mqtt_controller.h"
#include "services/mqtt_service.h"
#include "controllers/network_controller.h"
#include "controllers/sensor_controller.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "config/device_config.h"
#include "config/topics_config.h"
#include "config/watering_config.h"
#include "utils/json_parser.h"
#include <avr/eeprom.h>


#define JSON_BUF_SIZE 512
static char _json_buf[JSON_BUF_SIZE];

void pot_service_init(void)
{
}

void pot_service_handle_activate(const char *topic, const uint8_t *payload, uint16_t len)
{
    set_telemetry_enabled(true);

    logger_service_log("Pot ACTIVATED");

    uint32_t frequency = 0, dosage = 0;
    if (parse_watering_payload((const char *)payload, &frequency, &dosage))
    {
        update_watering_settings(frequency, dosage);
        logger_service_log("Watering settings updated and saved to EEPROM");
        log_stored_watering_config();
        set_watering_enabled(true);
    }
    else
    {
        logger_service_log("Failed to parse watering payload");
    }

    const char *ack = "{\"status\":\"ok\"}";

    unsigned char buffer[JSON_BUF_SIZE];

    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_service_publish(response_topic, (const uint8_t *)ack, buffer, sizeof(buffer));
}

void pot_service_handle_deactivate(const char *topic, const uint8_t *payload, uint16_t len)
{
    set_telemetry_enabled(false);
    set_watering_enabled(false);

    logger_service_log("Pot DEACTIVATED");

    const char *ack = "{\"status\":\"ok\"}";

    unsigned char buffer[JSON_BUF_SIZE];

    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_service_publish(response_topic, (const uint8_t *)ack, buffer, sizeof(buffer));
}

bool pot_service_handle_get_pot_data(const char *topic, const uint8_t *payload, uint16_t len)
{
    logger_service_log("Pot data requested");

    sensor_controller_poll();
    uint8_t hum_i = sensor_controller_get_humidity_integer();
    uint8_t hum_d = sensor_controller_get_humidity_decimal();
    uint8_t tmp_i = sensor_controller_get_temperature_integer();
    uint8_t tmp_d = sensor_controller_get_temperature_decimal();
    uint16_t light = sensor_controller_get_light();
    uint8_t soil = sensor_controller_get_soil();

    logger_service_log("Pot data!!!!: %u.%u %u.%u %u %u\n", tmp_i, tmp_d, hum_i, hum_d, light, soil);

    uint32_t light_lux = (light * 10000) / 1023;
    uint8_t soil_percentage = (soil * 100) / 1023;

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
                        (unsigned)soil_percentage, DEVICE_ID, 42, 42);

    logger_service_log("Pot data is built...\n");

    if (jlen <= 0 || jlen >= sizeof(_json_buf))
    {
        return false;
    }
    logger_service_log("The length is ok...\n");

    unsigned char buffer[JSON_BUF_SIZE];

    logger_service_log("Pot JSON payload: %s\n", _json_buf);

    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    WIFI_ERROR_MESSAGE_t result = mqtt_service_publish(response_topic, (unsigned char *)_json_buf, buffer, sizeof(buffer));

    if (result != WIFI_OK)
    {
        logger_service_log("Pot data publish failed\n");
        return false;
    }

    logger_service_log("Pot data published successfully\n");
    return true;
}