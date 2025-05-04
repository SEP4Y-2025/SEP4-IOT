// wifi_service.c (outline)

#include "services/wifi_service.h"
#include "controllers/network_controller.h"
#include "services/logger_service.h"
#include "services/logger_service.h"
#include "scheduler.h"     // for scheduler_millis()

typedef enum { DISCONNECTED, CONNECTING, CONNECTED } wifi_state_t;

static wifi_state_t state;
static const char *ssid, *pass;
static uint32_t      last_ms;

void wifi_service_init(const char *s, const char *p) {
    ssid        = s;
    pass        = p;
    state       = DISCONNECTED;
    last_ms     = scheduler_millis();
    network_controller_init();
    network_controller_setup();

    logger_service_log("Wi-Fi init: SSID=\"%s\"", ssid);
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
          logger_service_log("Wi-Fi: trying to connect to \"%s\"", ssid);
          network_controller_connect_ap(ssid, pass);
          state   = CONNECTING;
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
    }
    if (state != old) {
      static const char *names[] = { "DISCONNECTED", "CONNECTING", "CONNECTED" };
      logger_service_log("Wi-Fi state: %s → %s", names[old], names[state]);
  }

    //logger_service_log("Wifi state is: %s", state);
}
