#include "watering_service.h"
#include "config/device_config.h"
#include "services/logger_service.h"
#include "config/watering_config.h"
#include "utils/json_parser.h"
#include "state/watering_state.h"
#define JSON_BUF_SIZE 512
static char _json_buf[JSON_BUF_SIZE];

void watering_service_init(void)
{

}
void watering_service_water_pot(void)
{
    if(is_watering_enabled())
    {
        uint32_t dosage = get_water_dosage();
        uint32_t tank_capacity = WATER_TANK_CAPACITY_ML;

        logger_service_log("\nWatering pot...\n");

        //For Mathias: your watering code goes here
        //This function should just water the pot with the correct quantity of water, it is already set to repeat
        //Check initializer.c
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