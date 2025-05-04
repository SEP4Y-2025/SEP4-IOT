// services/sensor_service.c
#include "services/sensor_service.h"
#include "controllers/sensor_controller.h"
#include "scheduler.h"      // your millisecond scheduler

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
    }
}

bool sensor_service_has_new_data(void) {
    return _new_data;
}

float sensor_service_get_temperature(void) {
    _new_data = false;           // consume the data
    return sensor_controller_get_temperature();
}
float sensor_service_get_humidity(void) {
    _new_data = false;
    return sensor_controller_get_humidity();
}
uint16_t sensor_service_get_light(void) {
    _new_data = false;
    return sensor_controller_get_light();
}
