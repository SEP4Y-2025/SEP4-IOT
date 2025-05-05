// src/services/logger_service.c
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

void logger_service_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(_logger_buf, LOGGER_BUF_SZ - 3, fmt, args);
    va_end(args);

    if (n < 0) return;
    // append CRLF
    int len = (n < LOGGER_BUF_SZ - 3 ? n : LOGGER_BUF_SZ - 3);
    _logger_buf[len++] = '\r';
    _logger_buf[len++] = '\n';
    _logger_buf[len]   = '\0';

    pc_comm_controller_send_str(_logger_buf);
}
