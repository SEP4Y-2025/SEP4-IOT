#include "initializer.h"
#include "services/wifi_service.h"
#include "services/telemetry_service.h"
#include "services/logger_service.h"
#include "services/sensor_service.h"
#include "services/mqtt_service.h"
//#include "controllers/network_controller.h"
#include "scheduler.h"
#include "wifi.h"
#include <stdio.h>
#include <string.h>
#include "stdbool.h"
#include <stdint.h>

#define SENSOR_READ_INTERVAL 2000 // read sensors every 2 s
#define TELEMETRY_INTERVAL 60000  // publish once a minute

void initialize_system(void)
{
    scheduler_init();
    logger_service_init(9600);

    logger_service_log("Started sensor initialization");
    sensor_service_init(SENSOR_READ_INTERVAL);

    logger_service_log("Started telemetry initialization");
    telemetry_service_init();
}

WIFI_ERROR_MESSAGE_t setup_network_connection(char *ssid, char *password, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer)
{
    logger_service_log("Connecting to WiFi...\n");
    // initialize the wifi
    wifi_service_init();
    return wifi_service_connect(ssid, password, broker_ip, broker_port, callback, callback_buffer);
}