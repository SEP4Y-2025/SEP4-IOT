#include "scheduler.h"
#include "services/wifi_service.h"
//#include "controllers/sensor_controller.h"
// … other includes …

#define SENSOR_READ_INTERVAL   2000  // e.g. read sensors every 2 s
#define TELEMETRY_INTERVAL     60000 // publish once a minute

int main(void) {
    // 1) Init your 1 ms tick
    scheduler_init();

    // 2) Init other hardware/services
    wifi_service_init("SSID", "PASS");
    sensor_controller_init();
    // …

    // 3) Timestamp variables for each job
    uint32_t last_sensor_read = 0;
    uint32_t last_telemetry  = 0;

    while (1) {
        // 4) Drive Wi-Fi state machine
        wifi_service_poll();

        // 5) Sensor polling every SENSOR_READ_INTERVAL ms
        if (scheduler_elapsed(&last_sensor_read, SENSOR_READ_INTERVAL)) {
            sensor_controller_poll();
            scheduler_mark(&last_sensor_read);
        }

        // 6) Telemetry every TELEMETRY_INTERVAL ms
        if (scheduler_elapsed(&last_telemetry, TELEMETRY_INTERVAL)) {
            telemetry_service_publish();
            scheduler_mark(&last_telemetry);
        }

        // 7) …other work…
    }
}
