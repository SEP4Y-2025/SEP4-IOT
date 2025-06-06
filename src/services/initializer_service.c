#include "initializer_service.h"
#include "services/wifi_service.h"
#include "services/telemetry_service.h"
#include "services/logger_service.h"
#include "services/sensor_service.h"
#include "services/mqtt_service.h"
#include "scheduler.h"
#include "wifi.h"
#include <stdio.h>
#include <string.h>
#include "stdbool.h"
#include <stdint.h>
#include "config/device_config.h"
#include "services/pot_service.h"
#include <includes.h>
#include "config/topics_config.h"
#include "state/watering_state.h"
#include "services/watering_service.h"
#include "config/wifi_credentials.h"

#include "services/wifi_service.h"
#include "controllers/network_controller.h"

#define MQTT_PING_INTERVAL 5000
#define MQTT_POLL_INTERVAL_MS 15000
#define TELEMETRY_PUBLISH_INTERVAL 30000
#define WIFI_POLL_INTERVAL_MS 1000

void initializer_service_initialize_system(void)
{
    // Initialize scheduler
    scheduler_init();

    // Initialize the network controller *once* with MQTT callback + buffer
    network_controller_init(
        mqtt_service_event_callback,
        callback_buff);

    wifi_service_init();                                 // initialized with wifi_credentials.h
    mqtt_service_init(mqtt_broker_ip, mqtt_broker_port); // from wifi_credentials.h
    LOG("Connected to WiFi and MQTT broker!\n");

    // Initialize logger
    logger_service_init(9600);
    LOG("Logger initialized");

    watering_service_init();
    LOG("Watering service initialized");

    // Start sensor initialization
    LOG("Started sensor initialization");
    sensor_service_init();

    // Start telemetry initialization
    LOG("Started telemetry initialization");
    telemetry_service_init();

    // Wait before subscribing to topics
    _delay_ms(5000);
    // mqtt_service_subscribe_to_all_topics();
    load_watering_state(); // Load watering settings from EEPROM

    scheduler_register(wifi_service_poll, WIFI_POLL_INTERVAL_MS);
    scheduler_register(mqtt_service_poll, MQTT_POLL_INTERVAL_MS);
    scheduler_register(telemetry_service_publish, TELEMETRY_PUBLISH_INTERVAL);

    // For Mathias: the watering_frequency  is in hours
    // scheduler_register(watering_service_water_pot, get_watering_frequency() * 3600000); // Convert hours to milliseconds
    scheduler_register(watering_service_water_pot, get_watering_frequency() * 1000); // Use seconds for testing

    while (1)
    {
        scheduler_run_pending_tasks();
        _delay_ms(10); // avoid tight loop
    }
}

WIFI_ERROR_MESSAGE_t initializer_service_setup_network_connection(void (*callback)(void), char *callback_buffer)
{
    LOG("Connecting to WiFi...\n");
    wifi_service_init(callback, callback_buffer);
    return wifi_service_connect();
}
