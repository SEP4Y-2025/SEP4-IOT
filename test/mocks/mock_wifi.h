#ifndef MOCK_WIFI_H
#define MOCK_WIFI_H

#include <string.h>
#include <stdio.h>
#include "mock_uart.h"  // Use the mock UART here
#include <wifi.h>

static char mock_wifi_data_buffer[1024];
static uint16_t mock_wifi_data_index = 0;

void wifi_init() {
    // Call mock uart_init for WiFi USART with no callback initially
    uart_init(USART_WIFI, 115200, NULL);
}

void wifi_clear_databuffer_and_index() {
    memset(mock_wifi_data_buffer, 0, sizeof(mock_wifi_data_buffer));
    mock_wifi_data_index = 0;
}

WIFI_ERROR_MESSAGE_t wifi_command(const char *str, uint16_t timeOut_s) {
    printf("[MOCK WIFI] Sending command: %s\n", str);
    
    wifi_clear_databuffer_and_index();
    
    // Simulate reception of "OK\r\n" after command for testing success
    strcpy(mock_wifi_data_buffer, "OK\r\n");
    mock_wifi_data_index = (uint16_t)strlen(mock_wifi_data_buffer);
    
    // Simulate calling uart_init with callback (ignored in mock)
    uart_init(USART_WIFI, 115200, NULL);
    
    // Return success for test
    return WIFI_OK;
}

// Define other wifi_command_* functions similarly if needed, e.g.:

WIFI_ERROR_MESSAGE_t wifi_command_AT() {
    return wifi_command("AT", 1);
}

WIFI_ERROR_MESSAGE_t wifi_command_disable_echo() {
    return wifi_command("ATE0", 1);
}

#endif // MOCK_WIFI_H
