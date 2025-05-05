// #include "services/command_service.h"
// #include "controllers/network_controller.h"
// #include "controllers/mqtt_client.h"
// #include "services/mqtt_service.h"
// #include <string.h>

// #define MAX_HANDLERS  8

// static struct {
//     const char       *topic;
//     command_handler_t handler;
// } _handlers[MAX_HANDLERS];

// static int _handler_count = 0;

// // Internal MQTT callback: runs whenever mqtt_client_poll() sees a PUBLISH.
// static void _on_mqtt_message(const char *topic,
//                              const uint8_t *payload,
//                              uint16_t len)
// {
//     for (int i = 0; i < _handler_count; i++) {
//         if (strcmp(topic, _handlers[i].topic) == 0) {
//             _handlers[i].handler(topic, payload, len);
//             return;
//         }
//     }
// }

// bool command_service_register(const char *topic,
//                               command_handler_t handler)
// {
//     if (_handler_count >= MAX_HANDLERS) return false;
//     _handlers[_handler_count++] = (typeof(_handlers[0])){ topic, handler };
//     return true;
// }

// bool command_service_init(void) {
//     // 1) Ensure MQTT session is established
//     if (!mqtt_service_connect()) {
//         return false;
//     }

//     // 2) Tell the MQTT layer to invoke our dispatcher
//     mqtt_service_set_message_callback(_on_mqtt_message);

//     // 3) Subscribe to each registered topic
//     for (int i = 0; i < _handler_count; i++) {
//         if (!mqtt_service_subscribe(_handlers[i].topic)) {
//             // you may log or handle subscribe failures here
//             return false;
//         }
//     }
//     return true;
// }

// void command_service_poll(void)
// {
//     mqtt_client_poll();
// }
