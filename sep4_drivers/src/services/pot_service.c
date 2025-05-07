#include "services/pot_service.h"
#include "services/logger_service.h"
#include "services/telemetry_service.h"
//#include "controllers/mqtt_controller.h"
#include "services/mqtt_service.h"
#include "controllers/network_controller.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "services/device_config.h"

static bool telemetry_enabled = false;

void pot_service_init(void) {
}

bool pot_service_is_enabled(void) {
    return telemetry_enabled;
}

void pot_service_handle_activate(const char *topic, const uint8_t *payload, uint16_t len) {
    telemetry_enabled = true;
    logger_service_log("Pot ACTIVATED");

    const char *ack = "{\"status\":\"ok\"}";

    unsigned char buffer[128];

    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_service_publish(response_topic, (const uint8_t *)ack, buffer, sizeof(buffer));
}

void pot_service_handle_deactivate(const char *topic, const uint8_t *payload, uint16_t len) {
    telemetry_enabled = false;

    logger_service_log("Pot DEACTIVATED");

    const char *ack = "{\"status\":\"ok\"}";

    unsigned char buffer[128];

    char response_topic[64];
    snprintf(response_topic, sizeof(response_topic), "%s/ok", topic);

    mqtt_service_publish(response_topic, (const uint8_t *)ack, buffer, sizeof(buffer));
}
