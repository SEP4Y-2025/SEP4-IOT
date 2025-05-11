#ifndef LOGGER_SERVICE_H
#define LOGGER_SERVICE_H

#include <stdint.h>

/**
 * @brief Initialize the logger (sets up UART).
 * @param baudrate e.g. 115200
 */
void logger_service_init(uint32_t baudrate);

/**
 * @brief Send a formatted log line to the PC, automatically appends "\r\n".
 *        Usage just like printf.
 */
void logger_service_log(const char *fmt, ...);

#endif
