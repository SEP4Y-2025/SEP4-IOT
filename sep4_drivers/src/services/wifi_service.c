// wifi_service.c (outline)

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

static const wifi_credential_t *known_nets;
static uint8_t known_nets_len;

void wifi_service_init_best(const wifi_credential_t *nets, uint8_t nets_len) {
  known_nets     = nets;
  known_nets_len = nets_len;
  state          = DISCONNECTED;
  last_ms        = scheduler_millis();

  network_controller_init();
  network_controller_setup();
  logger_service_log("Wi-Fi smart init: %u candidates", (unsigned)known_nets_len);
}

bool wifi_service_is_connected(void) {
    return state == CONNECTED;
}

void wifi_service_poll(void) {
    uint32_t now = scheduler_millis();
    wifi_state_t old = state;

    switch (state) {
      case DISCONNECTED:
        if (now - last_ms >= 5000) {
            if (network_controller_connect_best(
                    known_nets, known_nets_len, /* scan timeout */ 10
                )) {
                state = CONNECTING;
            } else {
                logger_service_log("All connects failed, retry in 5s");
            }
            last_ms = now;
        }
        break;

      case CONNECTING:
        if (now - last_ms >= 3000) {
          if (network_controller_is_ap_connected())
            state = CONNECTED;
          else
            state = DISCONNECTED;
          last_ms = now;
        }
        break;

      case CONNECTED:
        if (now - last_ms >= 30000) {
          logger_service_log("Wi-Fi: checking still connected");
          if (!network_controller_is_ap_connected())
            state = DISCONNECTED;
          last_ms = now;
        }
        break;
      
      default:
        // If somehow we end up outside 0–2, force back to DISCONNECTED
        state = DISCONNECTED;
        last_ms = now;
        break;
    }
    // Only log when the state actually changes
  if (state != old) {
    // Now print the human‐readable form, clamping into range
    const char *old_name = (old < WIFI_STATE_MAX)
        ? WIFI_STATE_NAMES[old] : "UNKNOWN";
    const char *new_name = (state < WIFI_STATE_MAX)
        ? WIFI_STATE_NAMES[state] : "UNKNOWN";

    logger_service_log(
        "Wi-Fi state: %s → %s",
        old_name, new_name
    );
  }

    //logger_service_log("Wifi state is: %s", state);
}
