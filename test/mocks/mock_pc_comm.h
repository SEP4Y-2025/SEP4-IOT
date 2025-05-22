#ifndef MOCK_PC_COMM_H
#define MOCK_PC_COMM_H

#include <stdint.h>

typedef void (*pc_comm_callback_t)(uint8_t);

void pc_comm_init(uint32_t baudrate, pc_comm_callback_t char_received_callback);
void pc_comm_send_array_blocking(uint8_t *data, uint16_t length);
void pc_comm_send_string_blocking(char *string);
void pc_comm_send_array_nonBlocking(uint8_t * data, uint16_t length);

// Logging for tests
extern char mock_pc_comm_last_sent[256];

#endif
