#include "watering_service.h"
#include "config/device_config.h"
#include "services/logger_service.h"
#include "state/watering_state.h"
#include "controllers/sensor_controller.h" 
#include <pins_arduino.h>
#include <stdio.h>
#include "controllers/pump_controller.h"
#include "state/watering_state.h"
#include <stdint.h>
#include <stdbool.h>


void watering_service_init(void)
{
    pump_controller_init();
}

void watering_service_water_pot(void)
{

    if(is_watering_enabled() == false)
    {
        logger_service_log("Watering is disabled. \n");
        return;
    }

     char buffer[64];
     snprintf(buffer, sizeof(buffer), "Rain water level: %d\n", sensor_controller_get_water_level_raw());
     logger_service_log(buffer);

     snprintf(buffer, sizeof(buffer), "Rain water level percentage: %d\n", sensor_controller_get_water_level_percentage());


     uint8_t water_level = sensor_controller_get_water_level_percentage(); 

     logger_service_log(buffer);
    if (water_level >= 30)
    {
        //uint32_t dosage = get_water_dosage();  // Amount of water to use
        double dosage = 25.0; 
        logger_service_log("Watering pot...\n");

        uint32_t watering_time_ms = dosage / 240000 * 3600 * 1000 * 2 ; 
        logger_service_log("Watering time: %lu ms", watering_time_ms);
        activate_pump(watering_time_ms); 

        //TODO: notify the backend about the watering event
    }
    else 
    {
        logger_service_log("Not enough water available. Skipping watering.\n");
        //TODO: notify the backend about the low water level
    }
}
