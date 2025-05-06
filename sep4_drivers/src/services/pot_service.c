#include "services/pot_service.h"
#include "services/logger_service.h"
#include "services/telemetry_service.h"
//#include "controllers/mqtt_controller.h"
#include "services/mqtt_service.h"
#include "controllers/network_controller.h"
#include <stdbool.h>
#include <string.h>

static bool telemetry_enabled = false;


void pot_service_init(void) {
}

bool pot_service_is_enabled(void) {
    return telemetry_enabled;
}

void pot_service_handle_activate(const char *topic, const uint8_t *payload, uint16_t len) {
    telemetry_enabled = true;
    logger_service_log("Pot 1 ACTIVATED");

    const char *ack = "{\"status\":\"activated\"}";
    //unsigned char buf[128];
    mqtt_service_publish("/pot_1/activate/ok", (const uint8_t *)ack, strlen(ack));

    // size_t pktlen = mqtt_controller_build_publish_packet("/pot_1/activate/ok", (const unsigned char *)ack, strlen(ack), buf, sizeof(buf));
    // network_controller_tcp_send(buf, pktlen);
}

void pot_service_handle_deactivate(const char *topic, const uint8_t *payload, uint16_t len) {
    telemetry_enabled = false;
    logger_service_log("Pot 1 DEACTIVATED");

    const char *ack = "{\"status\":\"deactivated\"}";
    mqtt_service_publish("/pot_1/deactivate/ok", (const uint8_t *)ack, strlen(ack));
    //unsigned char buf[128];
    // size_t pktlen = mqtt_controller_build_publish_packet("/pot_1/deactivate/ok", (const unsigned char *)ack, strlen(ack), buf, sizeof(buf));
    // network_controller_tcp_send(buf, pktlen);
}
