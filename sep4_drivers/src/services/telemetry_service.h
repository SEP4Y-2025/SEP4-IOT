// services/telemetry_service.h
#ifndef TELEMETRY_SERVICE_H
#define TELEMETRY_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Configure the telemetry service.
 * @param topic  MQTT topic to PUBLISH telemetry on (e.g. "plant/telemetry").
 */
void telemetry_service_init(const char *topic);

/**
 * @brief Poll the MQTT service to drain CONNACK, PINGRESP, etc.
 *        Call this every loop (e.g. 1–10 ms cadence).
 */
void telemetry_service_poll(void);

/**
 * @brief Publish the latest sensor values as JSON:
 *        {"temperature":23.7,"humidity":45.2,"light":502,"soil":123}
 * @return true if the PUBLISH was sent (or at least queued) successfully.
 */
bool telemetry_service_publish(void);

#endif // TELEMETRY_SERVICE_H
