// mock_pc_comm.c
#include "mock_pc_comm.h"
#include <string.h>

char mock_pc_comm_last_sent[256] = {0};
static pc_comm_callback_t mock_callback;

void pc_comm_init(uint32_t baudrate, pc_comm_callback_t char_received_callback) {
    mock_callback = char_received_callback;
}

void pc_comm_send_array_blocking(uint8_t *data, uint16_t length) {
    memcpy(mock_pc_comm_last_sent, data, length);
    mock_pc_comm_last_sent[length] = '\0';
}

void pc_comm_send_string_blocking(char *string) {
    strncpy(mock_pc_comm_last_sent, string, sizeof(mock_pc_comm_last_sent));
}

void pc_comm_send_array_nonBlocking(uint8_t * data, uint16_t length) {
    memcpy(mock_pc_comm_last_sent, data, length);
    mock_pc_comm_last_sent[length] = '\0';
}
