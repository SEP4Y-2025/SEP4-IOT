#include "watering_service.h"
#include "config/device_config.h"
#include "services/logger_service.h"
#include "config/watering_config.h"
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

        logger_service_log("Watering pot...\n");

        //For Mathias: your watering code goes here
        //This function should just water the pot with the correct quantity of water, it is already set to repeat
        //Check initializer.c
    }
}

void watering_service_handle_settings_update(const char *topic, const uint8_t *payload, uint16_t len)
{
    //  Handle the watering update command, depending on the payload
    /////////////////// Change This //////////////////
    update_watering_settings(0, 0); // Dummy call 
    //////////////////////////////////////////////////



    
    const char *ack = "{\"status\":\"ok\"}";

    unsigned char buffer[JSON_BUF_SIZE];

    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_service_publish(response_topic, (const uint8_t *)ack, buffer, sizeof(buffer));
}