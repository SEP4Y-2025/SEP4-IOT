#include "controllers/pc_comm_controller.h"
#include "pc_comm.h" 

void pc_comm_controller_init(uint32_t baudrate, pc_comm_callback_t cb) {
    pc_comm_init(baudrate, cb);
}

bool pc_comm_controller_send(const uint8_t *data, uint16_t len) {
    pc_comm_send_array_blocking((uint8_t*)data, len);
    return true;
}

bool pc_comm_controller_send_str(const char *s) {
    pc_comm_send_string_blocking((char*)s);
    return true;
}
