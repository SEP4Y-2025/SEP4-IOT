#ifndef MOCK_UART_H
#define MOCK_UART_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

// Mock enums and types (copy from your original headers)
typedef enum { USART_0, USART_1, USART_2, USART_3, USART_WIFI } USART_t;
typedef void (*UART_Callback_t)(uint8_t);

static UART_Callback_t mock_usart0_rx_callback = NULL;
static UART_Callback_t mock_usart1_rx_callback = NULL;
static UART_Callback_t mock_usart2_rx_callback = NULL;
static UART_Callback_t mock_usart3_rx_callback = NULL;
static UART_Callback_t mock_usart_wifi_rx_callback = NULL;

UART_Callback_t uart_get_rx_callback(USART_t usart) {
    switch (usart) {
        case USART_0: return mock_usart0_rx_callback;
        case USART_1: return mock_usart1_rx_callback;
        case USART_2: return mock_usart2_rx_callback;
        case USART_3: return mock_usart3_rx_callback;
        case USART_WIFI: return mock_usart_wifi_rx_callback;
        default: return NULL;
    }
}

void uart_init(USART_t usart, uint32_t baudrate, UART_Callback_t callback) {
    // Save the callback in the mock variable
    switch (usart) {
        case USART_0: mock_usart0_rx_callback = callback; break;
        case USART_1: mock_usart1_rx_callback = callback; break;
        case USART_2: mock_usart2_rx_callback = callback; break;
        case USART_3: mock_usart3_rx_callback = callback; break;
        case USART_WIFI: mock_usart_wifi_rx_callback = callback; break;
        default: break;
    }
}

// Simulate sending a byte - store or print or count for tests
void uart_send_blocking(USART_t usart, uint8_t data) {
    // For mock, just print for visibility or store in buffer for test validation
    printf("[MOCK UART SEND] USART_%d: 0x%02X '%c'\n", usart, data, (data >= 32 && data <= 126) ? data : '.');
}

// Simulate sending an array
void uart_send_array_blocking(USART_t usart, uint8_t *data, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        uart_send_blocking(usart, data[i]);
    }
}

void uart_send_string_blocking(USART_t usart, char *data) {
    uart_send_array_blocking(usart, (uint8_t*)data, (uint16_t)strlen(data));
}

// Simulate receiving a byte from external source (call ISR callback)
void uart_mock_receive_byte(USART_t usart, uint8_t byte) {
    UART_Callback_t cb = uart_get_rx_callback(usart);
    if (cb) {
        cb(byte);
    }
}

#endif // MOCK_UART_H
