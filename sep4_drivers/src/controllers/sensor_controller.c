// controllers/sensor_controller.c
#include "controllers/sensor_controller.h"
#include "dht11.h"    // your low-level DHT22 driver
#include "light.h"
#include "soil.h"
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
    _light = light_read();
    _soil = soil_read();
}

float sensor_controller_get_temperature(void) {
    return _temp[0] + (_temp[1]* 0.1f);
}

float sensor_controller_get_humidity(void) {
    return _hum[0] + (_hum[1]* 0.1f);
}

uint16_t sensor_controller_get_soil(void) {
    return _soil;
}

uint16_t sensor_controller_get_light(void) {
    return _light;
}
