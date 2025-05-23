
#include "controllers/sensor_controller.h"
#include "dht11.h"   
#include "light.h"
#include "soil.h"
#include "rain_water_level_sensor.h" 
#include "services/logger_service.h"
#include <stdint.h>


static uint8_t    _temp[2];
static uint8_t    _hum[2];
static uint16_t _light;
static uint16_t _soil;

void sensor_controller_init(void) {
    dht11_init();   // sets up the data pin, timing, etc.
    light_init();
    soil_init();
    rain_water_level_sensor_init();  // Initialize the rain water level sensor
}

void sensor_controller_read(void) {
    DHT11_ERROR_MESSAGE_t err = dht11_get(&_hum[0], &_hum[1], &_temp[0], &_temp[1]);
    
    _light = light_read();
    _soil = soil_read();
}

uint8_t sensor_controller_get_temperature_integer(void) {
    return _temp[0];
}

uint8_t sensor_controller_get_temperature_decimal(void) {
    return _temp[1];
}

uint8_t sensor_controller_get_humidity_integer(void) {
    return _hum[0];
}

uint8_t sensor_controller_get_humidity_decimal(void) {
    return _hum[1];
}

uint16_t sensor_controller_get_soil(void) {
    return _soil;
}

uint16_t sensor_controller_get_light(void) {
    return _light;
}

uint16_t sensor_controller_get_water_level_raw(void) { 
    return rain_water_level_sensor_read_raw(); 
}

uint8_t sensor_controller_get_water_level_percentage(void) {  
    return rain_water_level_sensor_read_percentage();  
}
