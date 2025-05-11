
#ifndef TELEMETRY_SERVICE_H
#define TELEMETRY_SERVICE_H

#include <stdint.h>

/**
 * @brief  Configure the telemetry service.
 * */
void telemetry_service_init(void);

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
void telemetry_service_publish(void);

#endif 
