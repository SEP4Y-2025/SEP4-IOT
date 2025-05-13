#include "controllers/sensor_controller.h"
#include "dht11.h"    // your low-level DHT22 driver
#include "light.h"
#include "soil.h"
#include "services/logger_service.h"
#include "rain_water_level_sensor.h"  // Include the driver for the rain water level sensor

static uint8_t    _temp[2];
static uint8_t    _hum[2];
static uint16_t _light;
static uint16_t _soil;
static uint16_t _water_level;  // Variable to store the water level

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
    
    // Declare a variable to hold the water level
    uint8_t water_level = 0;

    // Read water level from the rain water level sensor
    if (rain_water_level_sensor_read(&water_level) == RAIN_SENSOR_OK) {
        _water_level = water_level;  // Store the water level
    } else {
        _water_level = 0;  // Handle failure (optional)
    }

    // Log the water level for debugging (optional)
    logger_service_log("Water level: %d", _water_level);  // New addition for logging
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

uint8_t sensor_controller_get_water_level(void) {  // New function for getting water level
    return _water_level;  // Return the last successful reading of the water level
}
