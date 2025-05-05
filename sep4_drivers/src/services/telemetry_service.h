// services/telemetry_service.h

#ifndef TELEMETRY_SERVICE_H
#define TELEMETRY_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief  Configure the telemetry service.
 *         Does not open the socket yet.
 *
 * @param broker_ip    Null-terminated broker IP or hostname
 * @param broker_port  TCP port (usually 1883)
 * @param client_id    MQTT client identifier
 * @param topic        MQTT topic to PUBLISH telemetry on
 */
void telemetry_service_init(const char   *broker_ip,
                            uint16_t      broker_port,
                            const char   *client_id,
                            const char   *topic);

/**
 * @brief  Poll the background TCP path.  
 *         Call this every loop (e.g. 1–10 ms cadence) to drain
 *         incoming bytes (CONNACK, etc.).
 */
void telemetry_service_poll(void);

/**
 * @brief  Publish the latest sensor values as JSON:
 *         { "temperature":23.7, "humidity":45.2, "light":502 }
 *
 *         On the very first call, it will open the TCP connection
 *         and send MQTT CONNECT.  Subsequent calls simply PUBLISH.
 *
 * @return true if the PUBLISH was sent (or scheduled) successfully.
 */
bool telemetry_service_publish(void);

#endif // TELEMETRY_SERVICE_H
