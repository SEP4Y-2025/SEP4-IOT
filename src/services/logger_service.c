#include "services/logger_service.h"
#include "controllers/pc_comm_controller.h"
#include <stdarg.h>
#include <stdio.h>

/// Maximum length of a single log line (including CRLF)
#define LOGGER_BUF_SZ 128

static char _logger_buf[LOGGER_BUF_SZ];

void logger_service_init(uint32_t baudrate) {
    // no RX callback needed for logger
    pc_comm_controller_init(baudrate, NULL);
}


void logger_service_log_internal(const char *file, int line, const char *fmt, ...) {
    if (!file || !fmt) return;

    // Print [file:line]
    int prefix_len = snprintf(_logger_buf, LOGGER_BUF_SZ - 1, "[%s:%d]\r\n", file, line);
    if (prefix_len < 0 || prefix_len >= LOGGER_BUF_SZ - 1) return;

    // Format the actual message below the file:line
    va_list args;
    va_start(args, fmt);
    int msg_len = vsnprintf(_logger_buf + prefix_len, LOGGER_BUF_SZ - prefix_len - 3, fmt, args);  // Leave space for \r\n\0
    va_end(args);

    if (msg_len < 0) return;

    int total_len = prefix_len + (msg_len < LOGGER_BUF_SZ - prefix_len - 3 ? msg_len : LOGGER_BUF_SZ - prefix_len - 3);
    _logger_buf[total_len++] = '\r';
    _logger_buf[total_len++] = '\n';
    _logger_buf[total_len] = '\0';

    pc_comm_controller_send_str(_logger_buf);
}

