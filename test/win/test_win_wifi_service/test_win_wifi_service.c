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
// Need to access static variable 'state' inside wifi_service.c for some tests.
// Declare it as extern here to manipulate state directly.
extern wifi_state_t state;

// Test poll() transitions DISCONNECTED -> CONNECTING after 5 seconds
void test_poll_disconnected_to_connecting_after_timeout(void)
{
    wifi_service_init();

    // simulate time passes more than 5000ms
    scheduler_millis_fake.return_val += 5001;

    network_controller_connect_ap_fake.return_val = WIFI_OK;

    wifi_service_poll();

    // Should have called network_controller_connect_ap()
    TEST_ASSERT_EQUAL_INT(1, network_controller_connect_ap_fake.call_count);
    // Should NOT be connected yet
    TEST_ASSERT_FALSE(wifi_service_is_connected());
    // State should now be CONNECTING
    TEST_ASSERT_EQUAL_INT(CONNECTING, state);
}

// Test poll() transitions CONNECTING -> DISCONNECTED on failure to connect
void test_poll_connecting_to_disconnected_on_fail(void)
{
    wifi_service_init();
    wifi_service_connect();

    // Simulate time passed > 3000ms
    scheduler_millis_fake.return_val += 3001;

    // Simulate failure: not connected
    network_controller_is_ap_connected_fake.return_val = false;

    wifi_service_poll();

    TEST_ASSERT_FALSE(wifi_service_is_connected());
    TEST_ASSERT_EQUAL_INT(DISCONNECTED, state);
}

// Test poll() transitions CONNECTED -> DISCONNECTED when connection lost
void test_poll_connected_to_disconnected_on_lost_connection(void)
{
    wifi_service_init();

    // Force state to CONNECTED
    state = CONNECTED;

    // Simulate time passed > 30000ms
    scheduler_millis_fake.return_val += 30001;

    // Simulate lost connection
    network_controller_is_ap_connected_fake.return_val = false;

    wifi_service_poll();

    TEST_ASSERT_FALSE(wifi_service_is_connected());
    TEST_ASSERT_EQUAL_INT(DISCONNECTED, state);
}

// Test poll() resets invalid state to DISCONNECTED
void test_poll_invalid_state_resets_to_disconnected(void)
{
    wifi_service_init();

    // Force invalid state
    state = WIFI_STATE_MAX + 10;

    scheduler_millis_fake.return_val += 1000;

    wifi_service_poll();

    TEST_ASSERT_FALSE(wifi_service_is_connected());
    TEST_ASSERT_EQUAL_INT(DISCONNECTED, state);
}

// Additional: test wifi_service_is_connected returns false when not CONNECTED
void test_wifi_service_is_connected_false_in_other_states(void)
{
    wifi_service_init();

    TEST_ASSERT_FALSE(wifi_service_is_connected());

    // Force state to DISCONNECTED
    state = DISCONNECTED;
    TEST_ASSERT_FALSE(wifi_service_is_connected());

    // Force state to CONNECTING
    state = CONNECTING;
    TEST_ASSERT_FALSE(wifi_service_is_connected());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_records_time_and_is_disconnected);
    RUN_TEST(test_connect_calls_controller_and_returns_status);
    RUN_TEST(test_poll_transitions_connecting_to_connected);
    RUN_TEST(test_poll_disconnected_to_connecting_after_timeout);
    RUN_TEST(test_poll_connecting_to_disconnected_on_fail);
    RUN_TEST(test_poll_connected_to_disconnected_on_lost_connection);
    RUN_TEST(test_poll_invalid_state_resets_to_disconnected);
    RUN_TEST(test_wifi_service_is_connected_false_in_other_states);
    return UNITY_END();
}
