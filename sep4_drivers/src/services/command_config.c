#include "services/command_config.h"
#include "services/command_service.h"
#include "services/pot_service.h"
#include "services/window_service.h"

void command_config_register_all(void) {
    command_service_register("plant/pot/add",    pot_service_handle_add);
    command_service_register("plant/pot/remove", pot_service_handle_remove);
    command_service_register("plant/window",     window_service_handle_toggle);
    // add more here as you grow…
}
