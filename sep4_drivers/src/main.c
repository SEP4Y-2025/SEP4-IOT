#include "scheduler.h"
#include "services/wifi_service.h"
#include "services/telemetry_service.h"
#include "services/sensor_service.h"
#include "services/logger_service.h"
//#include "controllers/sensor_controller.h"
// … other includes …

#define SENSOR_READ_INTERVAL   2000  // e.g. read sensors every 2 s
#define TELEMETRY_INTERVAL     60000 // publish once a minute

int main(void) {
    // 1) Init your 1 ms tick
    scheduler_init();

    // 2) Init other hardware/services
    logger_service_init(9600);
    
    logger_service_log("Wifi initialization");
    wifi_service_init("Kamtjatka_Only_For_Phones", "8755444387");
    
    logger_service_log("Sensor initialization");
    sensor_service_init(SENSOR_READ_INTERVAL);

    logger_service_log("MQTT initialization");
    telemetry_service_init(
        "192.168.1.100",   // broker IP
        1883,              // broker port
        "my_arduino",      // MQTT client ID
        "sensor/telemetry" // topic
    );

    // 3) Timestamp variables for each job
    uint32_t last_sensor_read = 0;
    uint32_t last_telemetry  = 0;

    logger_service_log("Starting the loop");
    while (1) {
        //scheduler_tick();
        // 4) Drive Wi-Fi state machine
        wifi_service_poll();
        //sensor_service_poll();

        // // 5) Sensor polling every SENSOR_READ_INTERVAL ms
        // if (scheduler_elapsed(&last_sensor_read, SENSOR_READ_INTERVAL)) {
        //     sensor_service_poll();
        //     scheduler_mark(&last_sensor_read);
        // }

        // // 6) Telemetry every TELEMETRY_INTERVAL ms
        // if (scheduler_elapsed(&last_telemetry, TELEMETRY_INTERVAL)) {
        //     telemetry_service_publish();
        //     logger_service_log("Telemetry sent");
        //     scheduler_mark(&last_telemetry);
        // }

        // 7) …other work…
    }
}
