
#include "services/wifi_service.h"
#include "controllers/network_controller.h"
#include "services/logger_service.h"
#include "services/logger_service.h"
#include "scheduler.h"     // for scheduler_millis()

typedef enum { DISCONNECTED = 0,
  CONNECTING   = 1,
  CONNECTED    = 2,
  WIFI_STATE_MAX
} wifi_state_t;

static const char *const WIFI_STATE_NAMES[WIFI_STATE_MAX] = {
  "DISCONNECTED",
  "CONNECTING",
  "CONNECTED"
};

static wifi_state_t state;
static const char *ssid, *pass;
static uint32_t      last_ms;

void wifi_service_init() {
    state       = DISCONNECTED;
    last_ms     = scheduler_millis();
    network_controller_init();
    network_controller_setup();

    logger_service_log("Wi-Fi init: SSID=\"%s\"", ssid);
}

WIFI_ERROR_MESSAGE_t wifi_service_connect(const char *s, const char *p, char *broker_ip, uint16_t broker_port, void (*callback)(void), char *callback_buffer) {
    ssid = s;
    pass = p;
    state = CONNECTING;
    last_ms = scheduler_millis();
    logger_service_log("Wi-Fi: connecting to \"%s\"", ssid);

  return network_controller_connect_ap(ssid, pass, broker_ip, broker_port, callback, callback_buffer);
}

bool wifi_service_is_connected(void) {
    return state == CONNECTED;
}

// void wifi_service_poll(void) {
//     uint32_t now = scheduler_millis();
//     wifi_state_t old = state;

//     switch (state) {
//       case DISCONNECTED:
//         if (now - last_ms >= 5000) {
//           logger_service_log("Wi-Fi: trying to connect to \"%s\"", ssid);
//           network_controller_connect_ap(ssid, pass);
//           state   = CONNECTING;
//           last_ms = now;
//         }
//         break;

//       case CONNECTING:
//         if (now - last_ms >= 3000) {
//           if (network_controller_is_ap_connected())
//             state = CONNECTED;
//           else
//             state = DISCONNECTED;
//           last_ms = now;
//         }
//         break;

//       case CONNECTED:
//         if (now - last_ms >= 30000) {
//           logger_service_log("Wi-Fi: checking still connected");
//           if (!network_controller_is_ap_connected())
//             state = DISCONNECTED;
//           last_ms = now;
//         }
//         break;
      
//       default:
//         // If somehow we end up outside 0–2, force back to DISCONNECTED
//         state = DISCONNECTED;
//         last_ms = now;
//         break;
//     }
//     // Only log when the state actually changes
//   if (state != old) {
//     // Now print the human‐readable form, clamping into range
//     const char *old_name = (old < WIFI_STATE_MAX)
//         ? WIFI_STATE_NAMES[old] : "UNKNOWN";
//     const char *new_name = (state < WIFI_STATE_MAX)
//         ? WIFI_STATE_NAMES[state] : "UNKNOWN";

//     logger_service_log(
//         "Wi-Fi state: %s → %s",
//         old_name, new_name
//     );
//   }

//     //logger_service_log("Wifi state is: %s", state);
// }
