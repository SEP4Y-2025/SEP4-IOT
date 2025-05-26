#include "watering_service.h"
#include "config/device_config.h"
#include "services/logger_service.h"
#include "utils/json_parser.h"
#include "state/watering_state.h"
#include "controllers/sensor_controller.h"
#include "includes.h"
#include <stdio.h>
#include "controllers/pump_controller.h"
#include "state/watering_state.h"
#include <stdint.h>
#include <stdbool.h>
#include "services/mqtt_service.h"

#define JSON_BUF_SIZE 512
static char _json_buf[JSON_BUF_SIZE];
void watering_service_init(void)
{
    pump_controller_init();
}

void watering_service_water_pot(void)
{

    if (is_watering_enabled() == false)
    {
        LOG("Watering is disabled. \n");
        return;
    }

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Rain water level: %d\n", sensor_controller_get_water_level_raw());
    LOG(buffer);

    snprintf(buffer, sizeof(buffer), "Rain water level percentage: %d\n", sensor_controller_get_water_level_percentage());

    uint8_t water_level = sensor_controller_get_water_level_percentage();

    LOG(buffer);
    if (water_level >= 30)
    {
        // uint32_t dosage = get_water_dosage();  // Amount of water to use
        double dosage = 25.0;
        LOG("Watering pot...\n");

        uint32_t watering_time_ms = dosage / 240000 * 3600 * 1000 * 2;
        LOG("Watering time: %lu ms", watering_time_ms);
        activate_pump(watering_time_ms);
    }
    // TODO: notify the backend about the watering event
    else
    {
        LOG("Not enough water available. Skipping watering.\n");
    }
}
void watering_service_handle_settings_update(const char *topic, const uint8_t *payload, uint16_t len)
{

    uint32_t freq = 0;
    uint32_t dosage = 0;
    if (parse_watering_payload((const char *)payload, &freq, &dosage))
    {
        set_telemetry_enabled(false);
        update_watering_settings(freq, dosage);
        LOG("Watering settings updated and saved to EEPROM");
        set_watering_enabled(true);
    }
    else
    {
        LOG("Failed to parse watering payload");
    }

    const char *ack = "{\"status\":\"ok\"}";

    unsigned char buffer[JSON_BUF_SIZE];

    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_error_t err = mqtt_service_publish(response_topic, ack);
    if (err != MQTT_OK)
    {
        LOG("Failed to publish ACK (err=%d)", err);
    }
}
