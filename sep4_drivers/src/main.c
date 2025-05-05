#include "scheduler.h"
#include "services/wifi_service.h"
#include "services/telemetry_service.h"
#include "services/sensor_service.h"
#include "services/logger_service.h"
#include <avr/io.h>
#include <avr/wdt.h>

#include "services/mqtt_service.h"
#include "services/pot_service.h"
// #include "controllers/sensor_controller.h"
//  … other includes …

#define SENSOR_READ_INTERVAL 2000 // e.g. read sensors every 2 s
#define TELEMETRY_INTERVAL 60000  // publish once a minute

int main(void)
{
    // 1) Init your 1 ms tick
    scheduler_init();

    // 2) Init other hardware/services
    logger_service_init(9600);

    logger_service_log("Wifi initialization");
    wifi_service_init("Betelgeuse", "Hello World");

    logger_service_log("Sensor initialization");
    sensor_service_init(SENSOR_READ_INTERVAL);

    logger_service_log("MQTT initialization");
    telemetry_service_init(
        "192.168.120.58",  // broker IP
        1883,              // broker port
        "pot_1",      // MQTT client ID
        "sensor/telemetry" // topic
    );

    mqtt_service_init();
    logger_service_log("MQTT service initialized");

    
    // 3) Timestamp variables for each job
    uint32_t last_sensor_read = 0;
    uint32_t last_telemetry = 0;

    logger_service_log("Starting the loop");
    mqtt_subscribe();
    while (1)
    {

        wifi_service_poll();
        mqtt_service_poll();
        // sensor_service_poll();
        telemetry_service_publish();

        if (scheduler_elapsed(&last_telemetry, SENSOR_READ_INTERVAL))
        {
            if (pot_service_is_enabled())
            {
                telemetry_service_publish();
                logger_service_log("Telemetry sent");
            }
            scheduler_mark(&last_telemetry);
        }
    }
}
