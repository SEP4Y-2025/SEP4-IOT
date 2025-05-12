#include "watering_service.h"
#include "config/device_config.h"
#include "services/logger_service.h"
#include "state/watering_state.h"

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