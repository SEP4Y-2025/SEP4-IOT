#include "scheduler.h"
#include "services/wifi_service.h"
#include "services/telemetry_service.h"
#include "services/sensor_service.h"
#include "services/initializer.h"
#include "services/logger_service.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include "MQTTPacket.h"
#include <util/delay.h>
#include <string.h>
#include "services/mqtt_service.h"
#include "services/pot_service.h"
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <wifi.h>

int main(void)
{
    initialize_system();
    logger_service_log("Initialization complete.\n");

    if (setup_network_connection("Betelgeuse", "Hello World", "192.168.120.58", 1883,mqtt_event_cb, callback_buff) != WIFI_OK)
    {
        logger_service_log("Error setting up network connection!\n");
        return -1;
    }

    logger_service_log("Connected to WiFi and MQTT broker!\n");

    _delay_ms(5000);
    subscribe_to_all_topics();

    // periodic_task_init_a(loop, 2000);
    // Keep-alive logic
    uint32_t last_ping_ms = scheduler_millis();
    while (1)
    {

        if (scheduler_elapsed(&last_ping_ms, 15000)) // Every 15 seconds
        {
            mqtt_send_pingreq(); // Send MQTT keep-alive
            scheduler_mark(&last_ping_ms);
        }
        _delay_ms(10);
    }

    return 0;
}
