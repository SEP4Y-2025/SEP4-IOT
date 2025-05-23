#include "services/initializer_service.h"
#include "services/logger_service.h"

int main(void)
{
#ifndef WINDOWS_TEST
    initializer_service_initialize_system();
#endif

    LOG("Initialization complete.\n");

    return 0;
}
