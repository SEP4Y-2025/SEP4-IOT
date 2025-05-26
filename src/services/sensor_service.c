
#include "services/sensor_service.h"
#include "controllers/sensor_controller.h"

void sensor_service_init() {
    sensor_controller_init();
}

bool sensor_service_read(void) {
    sensor_controller_read();
}

uint8_t sensor_service_get_temperature_integer(void) {
    return sensor_controller_get_temperature_integer();
}
uint8_t sensor_service_get_temperature_decimal(void) {
    return sensor_controller_get_temperature_decimal();
}
uint8_t sensor_service_get_humidity_integer(void) {
    return sensor_controller_get_humidity_integer();
}
uint8_t sensor_service_get_humidity_decimal(void) {
    return sensor_controller_get_humidity_decimal();
}
uint16_t sensor_service_get_light(void) {
    return sensor_controller_get_light();
}
uint16_t sensor_service_get_soil(void) {
    return sensor_controller_get_soil();
}

uint16_t sensor_service_get_water_level_raw(void) {
    return sensor_controller_get_water_level_raw();
}

uint8_t sensor_service_get_water_level_percentage(void) {
    return sensor_controller_get_water_level_percentage();
}
