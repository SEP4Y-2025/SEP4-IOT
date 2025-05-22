// tests/test_win_wifi_service/test_win_wifi_service.c

#include "unity.h"
#include "../fff.h"

// 1) Turn off LOG so it doesn’t call into logger_service
#ifdef LOG
#undef LOG
#endif
#define LOG(...)

// 2) Pull in the implementation under test (it itself includes
//    wifi_service.h → wifi.h, scheduler.h, network_controller.h, etc.)
#include "../../src/services/wifi_service.c"

// 3) Now declare FFF fakes for *all* externs wifi_service.c calls:

// scheduler_millis() → returns uint32_t
DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC(uint32_t, scheduler_millis);

// network_controller_connect_ap(uint16_t) → returns WIFI_ERROR_MESSAGE_t
FAKE_VALUE_FUNC(WIFI_ERROR_MESSAGE_t, network_controller_connect_ap, uint16_t);

// network_controller_is_ap_connected() → returns bool
FAKE_VALUE_FUNC(bool, network_controller_is_ap_connected);

void logger_service_log_internal(const char *file,
                                 int line,
                                 const char *fmt,
                                 ...)
{
    (void)file;
    (void)line;
    (void)fmt;
}
// 4) Test setup/teardown
void setUp(void)
{
    RESET_FAKE(scheduler_millis);
    RESET_FAKE(network_controller_connect_ap);
    RESET_FAKE(network_controller_is_ap_connected);
}
void tearDown(void) {}

// 5) The tests

// a) init() grabs the first timestamp and leaves DISCONNECTED
void test_init_records_time_and_is_disconnected(void)
{
    scheduler_millis_fake.return_val = 1000;
    wifi_service_init();
    TEST_ASSERT_EQUAL_INT(1, scheduler_millis_fake.call_count);
    TEST_ASSERT_FALSE(wifi_service_is_connected());
}

// b) connect() calls network_controller and returns its status
void test_connect_calls_controller_and_returns_status(void)
{
    scheduler_millis_fake.return_val = 2000;
    network_controller_connect_ap_fake.return_val = WIFI_FAIL;
    WIFI_ERROR_MESSAGE_t res = wifi_service_connect();
    TEST_ASSERT_EQUAL_INT(1, network_controller_connect_ap_fake.call_count);
    TEST_ASSERT_EQUAL_INT(WIFI_FAIL, res);
}

// c) poll(): from CONNECTING→CONNECTED when the controller says “yes”
void test_poll_transitions_connecting_to_connected(void)
{
    // force state to CONNECTING
    scheduler_millis_fake.return_val = 0;
    network_controller_connect_ap_fake.return_val = WIFI_OK;
    wifi_service_connect();

    // advance time past 3000ms, fake a successful link
    scheduler_millis_fake.return_val = 3001;
    network_controller_is_ap_connected_fake.return_val = true;
    wifi_service_poll();
    TEST_ASSERT_TRUE(wifi_service_is_connected());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_records_time_and_is_disconnected);
    RUN_TEST(test_connect_calls_controller_and_returns_status);
    RUN_TEST(test_poll_transitions_connecting_to_connected);
    return UNITY_END();
}
