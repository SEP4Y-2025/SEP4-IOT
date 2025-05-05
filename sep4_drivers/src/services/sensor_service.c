// services/sensor_service.c
#include "services/sensor_service.h"
#include "controllers/sensor_controller.h"
#include "scheduler.h"      // your millisecond scheduler
#include "services/logger_service.h"

static uint32_t  _interval;
static uint32_t  _last_ms;
static bool      _new_data;

void sensor_service_init(uint32_t interval_ms) {
    _interval = interval_ms;
    _last_ms  = 0;
    _new_data = false;
    sensor_controller_init();
}

void sensor_service_poll(void) {
    if (scheduler_elapsed(&_last_ms, _interval)) {
        sensor_controller_poll();
        _new_data = true;
        scheduler_mark(&_last_ms);
        logger_service_log("T=%u.%u°C H=%u.%u L=%u S=%u",
            sensor_service_get_temperature_integer(),
            sensor_service_get_temperature_decimal(),
            sensor_service_get_humidity_integer(),
            sensor_service_get_humidity_decimal(),
            sensor_service_get_light(),
            sensor_service_get_soil()
        );
    }
}

bool sensor_service_has_new_data(void) {
    return _new_data;
}

uint8_t sensor_service_get_temperature_integer(void) {
    _new_data = false;
    return sensor_controller_get_temperature_integer();
}
uint8_t sensor_service_get_temperature_decimal(void) {
    _new_data = false;
    return sensor_controller_get_temperature_decimal();
}
uint8_t sensor_service_get_humidity_integer(void) {
    _new_data = false;
    return sensor_controller_get_humidity_integer();
}
uint8_t sensor_service_get_humidity_decimal(void) {
    _new_data = false;
    return sensor_controller_get_humidity_decimal();
}
uint16_t sensor_service_get_light(void) {
    _new_data = false;
    return sensor_controller_get_light();
}
uint16_t sensor_service_get_soil(void) {
    _new_data = false;
    return sensor_controller_get_soil();
}
