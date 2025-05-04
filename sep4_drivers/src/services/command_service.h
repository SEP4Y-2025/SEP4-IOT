#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/** Prototype for your topic handlers. */
typedef void (*command_handler_t)(const char *topic,
                                  const uint8_t *payload,
                                  uint16_t len);

/**
 * Register a handler for a specific MQTT topic.
 * Must be called before command_service_init().
 * Returns false if you exceed the max handler count.
 */
bool command_service_register(const char *topic,
                              command_handler_t handler);

/**
 * @brief Initialize the command service.
 *        Must be called after mqtt_service_init().
 */
bool command_service_init(void);


/**
 * Drive the MQTT client and dispatch any incoming messages.
 * Call this inside your main loop.
 */
void command_service_poll(void);

#endif // COMMAND_SERVICE_H
