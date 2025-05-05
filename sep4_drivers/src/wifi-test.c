// // src/wifi-test.c

// #include <util/delay.h>     // for _delay_ms()
// #include "uart.h"
// #include "wifi.h"
// #include "display.h"
// #include "light.h"
// #include "mqtt.h"
// #include "periodic_task.h"
// #include "servo.h"
// #include "soil.h"
// #include "uart.h"
// #include "wifi.h"
// #include <avr/interrupt.h>
// #include <stdbool.h>
// #include <stddef.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <util/delay.h>

// static uint8_t _buff[100];
// static uint8_t _index = 0;
// volatile static bool _done = false;
// void console_rx(uint8_t _rx) {
//   uart_send_blocking(USART_0, _rx);
//   if (('\r' != _rx) && ('\n' != _rx)) {
//     if (_index < 100 - 1) {
//       _buff[_index++] = _rx;
//     }
//   } else {
//     _buff[_index] = '\0';
//     _index = 0;
//     _done = true;
//     uart_send_blocking(USART_0, '\n');
//     //        uart_send_string_blocking(USART_0, (char*)_buff);
//   }
// }

// int main(void) {
//     // 1) Init PC serial (USB-UART)
//     uart_init(USART_0, 9600, console_rx);
//     uart_send_string_blocking(USART_0, "Starting WiFi scan test...\r\n");

//     // 2) Init WiFi UART + module
//     wifi_init();

//     // if (wifi_command_disable_echo() != WIFI_OK) {
//     //     uart_send_string_blocking(USART_0, "Error: could not disable echo\r\n");
//     // }
//     // if (wifi_command_set_mode_to_1() != WIFI_OK) {
//     //     uart_send_string_blocking(USART_0, "Error: could not set station mode\r\n");
//     // }
//     // if (wifi_command_set_to_single_Connection() != WIFI_OK) {
//     //     uart_send_string_blocking(USART_0, "Error: could not set single connection\r\n");
//     // }

//     // 3) Test AT responsiveness
//     if (wifi_command_AT() == WIFI_OK) {
//         uart_send_string_blocking(USART_0, "ESP8266: AT OK\r\n");
//     } else {
//         uart_send_string_blocking(USART_0, "ESP8266: no AT response\r\n");
//     }

//     // 4) Do an initial scan
//     WIFI_ERROR_MESSAGE_t res = wifi_scan_APs(10);
//     if (res != WIFI_OK) {
//         uart_send_string_blocking(USART_0, "Scan failed\r\n");
//     }

//     // 5) Periodic rescans every 60s
//     while (1) {
//         _delay_ms(60000);
//         uart_send_string_blocking(USART_0, "\r\nRescanning APs...\r\n");
//         res = wifi_scan_APs(10);
//         if (res != WIFI_OK) {
//             uart_send_string_blocking(USART_0, "Scan failed\r\n");
//         }
//     }

//     return 0;
// }
