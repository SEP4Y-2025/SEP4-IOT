// src/controllers/pc_comm_controller.h
#ifndef PC_COMM_CONTROLLER_H
#define PC_COMM_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

/** Character‐received callback (unused by logger). */
typedef void (*pc_comm_callback_t)(char c);

/**
 * @brief Initialize the UART for PC communication.
 * @param baudrate Desired baud (e.g. 115200)
 * @param cb       Callback on RX (or NULL)
 */
void pc_comm_controller_init(uint32_t baudrate, pc_comm_callback_t cb);

/**
 * @brief Send a buffer of bytes (blocking).
 * @return true on success
 */
bool pc_comm_controller_send(const uint8_t *data, uint16_t len);

/**
 * @brief Send a NUL-terminated string (blocking).
 * @return true on success
 */
bool pc_comm_controller_send_str(const char *s);

#endif // PC_COMM_CONTROLLER_H
