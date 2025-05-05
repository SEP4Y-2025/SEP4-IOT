#include "scheduler.h"
#include "services/wifi_service.h"
#include "services/telemetry_service.h"
#include "services/sensor_service.h"
#include "services/logger_service.h"
#include "controllers/network_controller.h"
#include "controllers/mqtt_client.h"
#include "services/mqtt_service.h"
#include "services/command_config.h"
#include "services/command_service.h"
// #include "services/"

#include <avr/io.h>
#include <avr/wdt.h>
// #include "controllers/sensor_controller.h"
//  … other includes …

#define SENSOR_READ_INTERVAL 2000 // e.g. read sensors every 2 s
#define TELEMETRY_INTERVAL 60000  // publish once a minute

// { "Kamtjatka_Only_For_Phones",   "8755444387"   },
// { "JanPhone", "Hello World" }
// { "Kamtjatka10","8755444387" }

static const wifi_credential_t my_nets[] = {
    {"JanPhone", "Hello World"}};

static const mqtt_controller_config_t mqtt_cfg = {
    .client_id = "mega_iot_device",
    .keepalive_interval = 60,
    .cleansession = 1,
    .MQTTVersion = 4,
    .username = "",
    .password = "",
};

int main(void)
{
    // 1) Init your 1 ms tick
    scheduler_init();

    // 2) Init other hardware/services
    logger_service_init(9600);

    logger_service_log("Wifi initialization");
    wifi_service_init_best(my_nets, sizeof(my_nets) / sizeof(*my_nets));

    logger_service_log("Sensor initialization");
    sensor_service_init(SENSOR_READ_INTERVAL);

    logger_service_log("MQTT initialization");
    mqtt_service_init("172.20.10.3", 1883, &mqtt_cfg);

    logger_service_log("Telemetry initialization");
    telemetry_service_init("plant/telemetry");

    logger_service_log("Command registration and initialization");
    command_config_register_all();

    // 3) Timestamp variables for each job
    uint32_t last_sensor_read = 0;
    uint32_t last_telemetry = 0;

    logger_service_log("Starting the loop");
    while (1)
    {

        wifi_service_poll();

        // mqtt_service_poll();

        command_service_poll();

        // sensor_service_poll();
        // telemetry_service_publish();

        if (scheduler_elapsed(&last_telemetry, SENSOR_READ_INTERVAL))
        {
            telemetry_service_publish();
            // logger_service_log("Telemetry sent");
            scheduler_mark(&last_telemetry);
        }
    }
}
