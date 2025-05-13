#ifndef LOGGER_SERVICE_H
#define LOGGER_SERVICE_H

#include <stdint.h>
#define LOG(fmt, ...) \
    logger_service_log_internal(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

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

/**
 * @brief Send a formatted log line to the PC, automatically appends "\r\n".
 *        Usage just like printf.
 * @param file The name of the file where the log is called
 * @param func The name of the function where the log is called
 * @param line The line number where the log is called
 */
void logger_service_log_internal(const char *file, const char *func, int line, const char *fmt, ...);
#endif
