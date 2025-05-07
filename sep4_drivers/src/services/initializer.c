#include "initializer.h"
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

#define SENSOR_READ_INTERVAL 2000 // e.g. read sensors every 2 s
#define TELEMETRY_INTERVAL 60000  // publish once a minute

// static char ssid[32] = "Betelgeuse";
// static char password[32] = "Hello World";

void initialize_system(void)
{
    scheduler_init();
    logger_service_init(9600);

    // logger_service_log("Started wifi initialization");
    // wifi_service_init(ssid, password);

    logger_service_log("Started sensor initialization");
    sensor_service_init(SENSOR_READ_INTERVAL);

    logger_service_log("Started telemetry initialization");
    telemetry_service_init();
}

WIFI_ERROR_MESSAGE_t setup_network_connection(char *ssid, char *password, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer)
{

    logger_service_log("Connecting to WiFi...\n");
    // initialize the wifi module
    wifi_init();

    // disable echo, set station mode, single-conn
    if (wifi_command_disable_echo() != WIFI_OK)
        return -1;
    if (wifi_command_set_mode_to_1() != WIFI_OK)
        return -1;
    if (wifi_command_set_to_single_Connection() != WIFI_OK)
        return -1;
    // connect to wifi
    WIFI_ERROR_MESSAGE_t wifi_res = wifi_command_join_AP(ssid, password);

    // Log result of WiFi connection
    char wifi_res_msg[128];
    sprintf(wifi_res_msg, "Error: %d \n", wifi_res);
    logger_service_log(wifi_res_msg);

    if (wifi_res != WIFI_OK)
    {
        logger_service_log("Error connecting to WiFi!\n");
        return -1;
    }
    else
    {
        logger_service_log("Connected to WiFi!\n");
    }

    //wifi_service_init(ssid, password);

    // connect to tcp server
    wifi_command_create_TCP_connection(broker_ip, broker_port, callback, callback_buffer);

    // Create and send MQTT connect packet
    unsigned char connect_buf[200];
    int connect_buflen = sizeof(connect_buf);
    int connect_len = create_mqtt_connect_packet(connect_buf, connect_buflen);

    if (connect_len > 0)
    {
        logger_service_log("MQTT Connect packet created!\n");
    }

    WIFI_ERROR_MESSAGE_t mqtt_res = wifi_command_TCP_transmit(connect_buf, connect_len);

    if (mqtt_res != WIFI_OK)
    {
        logger_service_log("Error sending MQTT Connect packet!\n");
        return mqtt_res;
    }

    return WIFI_OK;
}