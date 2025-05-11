#include "services/initializer_service.h"
#include "services/logger_service.h"

int main(void)
{
    initializer_service_initialize_system();
    logger_service_log("Initialization complete.\n");

    return 0;
}
