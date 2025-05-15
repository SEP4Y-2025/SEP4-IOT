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
#include <util/delay.h>
#include "config/topics_config.h"
#include "state/watering_state.h"
#include "services/watering_service.h"

#define MQTT_PING_INTERVAL 15000 
#define TELEMETRY_PUBLISH_INTERVAL 10000 

void initializer_service_initialize_system(void)
{
    // Initialize scheduler
    scheduler_init();

    // Initialize logger
    logger_service_init(9600);
    logger_service_log("Logger initialized");

    // Start sensor initialization
    logger_service_log("Started sensor initialization");
    sensor_service_init();

    // Start telemetry initialization
    logger_service_log("Started telemetry initialization");
    telemetry_service_init();

    // Connect to Wi-Fi and MQTT broker
    if (initializer_service_setup_network_connection("WiFi6625rkm", "sGg7bj5CE", "192.168.1.182", 1883, mqtt_service_event_callback, callback_buff) != WIFI_OK)
    {
        logger_service_log("Error setting up network connection!\n");
        return;
    }

    logger_service_log("Connected to WiFi and MQTT broker!\n");

    // Wait before subscribing to topics
    _delay_ms(5000);
    mqtt_service_subscribe_to_all_topics();

    // Load watering settings from EEPROM
    load_watering_state(); 

    // Register periodic tasks for MQTT ping and telemetry
    scheduler_register(mqtt_service_send_pingreq, MQTT_PING_INTERVAL);          
    scheduler_register(telemetry_service_publish, TELEMETRY_PUBLISH_INTERVAL);     

    // Log watering frequency and register watering task with scheduler
    uint32_t watering_frequency_seconds = get_watering_frequency() * 3600;  // Convert hours to seconds
    logger_service_log("Watering frequency: %lu seconds", watering_frequency_seconds);

    // Register watering service to run periodically based on watering frequency
    scheduler_register(watering_service_water_pot, 10000);  // Convert to milliseconds

    // Main loop for running scheduled tasks
    while (1) {
        scheduler_run_pending_tasks();
        _delay_ms(10); // avoid tight loop
    }
}

WIFI_ERROR_MESSAGE_t initializer_service_setup_network_connection(char *ssid, char *password, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer)
{
    logger_service_log("Connecting to WiFi...\n");
    wifi_service_init();
    return wifi_service_connect(ssid, password, broker_ip, broker_port, callback, callback_buffer);
}
