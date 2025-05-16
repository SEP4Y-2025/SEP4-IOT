#include "services/initializer_service.h"
#include "services/logger_service.h"

int main(void)
{
    initializer_service_initialize_system();
    LOG("Initialization complete.\n");

    return 0;
}
