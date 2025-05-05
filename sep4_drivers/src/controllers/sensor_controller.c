// controllers/sensor_controller.c
#include "controllers/sensor_controller.h"
#include "dht11.h"    // your low-level DHT22 driver
#include "light.h"
#include "soil.h"
#include "services/logger_service.h"
//#include "ldr.h"      // your low-level LDR driver

static uint8_t    _temp[2];
static uint8_t    _hum[2];
static uint16_t _light;
static uint16_t _soil;

void sensor_controller_init(void) {
    dht11_init();   // sets up the data pin, timing, etc.
    light_init();
    soil_init();
    // ldr_init();     // configures the ADC channel for the photo-resistor
}

void sensor_controller_poll(void) {
    DHT11_ERROR_MESSAGE_t err = dht11_get(&_hum[0], &_hum[1], &_temp[0], &_temp[1]);
    
    // if (err == DHT11_OK) {
    //     // successful read → log the precise values
    //     logger_service_log(
    //         "DHT11 OK"
    //     );
    // } else {
    //     // failed read → log an error
    //     logger_service_log("DHT11 read FAILED");
    // }
    
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
